/* HMI Thread entry function */

#include "hal_data.h"
#include "system_thread.h"
#include "lcd.h"
#include "r_ioport_api.h"

void ReadLCD(uint8_t cmd, uint8_t* data ,uint32_t len);
void WriteLCD(uint8_t cmd, uint8_t* data ,uint32_t len);



void WriteLCD(uint8_t cmd, uint8_t* data ,uint32_t len)
{
	ssp_err_t err;
        ssp_err_t lcd_err;
	/* CMD and CS assert */
	g_ioport.p_api->pinWrite(LCD_CMD,IOPORT_LEVEL_LOW);
	g_ioport.p_api->pinWrite(LCD_CS,IOPORT_LEVEL_LOW);

	err = g_lcd_spi.p_api->write(g_lcd_spi.p_ctrl, &cmd, 1, SPI_BIT_WIDTH_8_BITS);
	if (SSP_SUCCESS != err)
	{
		while(1);
	}
	tx_semaphore_get(&g_lcd_spi_done,TX_WAIT_FOREVER);

	if (len)
	{
		lcd_err = g_ioport.p_api->pinWrite(LCD_CMD,IOPORT_LEVEL_HIGH);
		lcd_err = g_lcd_spi.p_api->write(g_lcd_spi.p_ctrl, data, len,SPI_BIT_WIDTH_8_BITS);
		tx_semaphore_get(&g_lcd_spi_done,TX_WAIT_FOREVER);
	}
	g_ioport.p_api->pinWrite(LCD_CS,IOPORT_LEVEL_HIGH);

}

void ReadLCD(uint8_t cmd, uint8_t* data ,uint32_t len)
{
        ssp_err_t lcd_err;
	static uint8_t dummy_write[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	lcd_err = g_ioport.p_api->pinWrite(LCD_CMD,IOPORT_LEVEL_LOW);
	lcd_err = g_ioport.p_api->pinWrite(LCD_CS,IOPORT_LEVEL_LOW);
	lcd_err = g_lcd_spi.p_api->write(g_lcd_spi.p_ctrl, &cmd, 1, SPI_BIT_WIDTH_8_BITS);
	tx_semaphore_get(&g_lcd_spi_done,TX_WAIT_FOREVER);

	lcd_err = g_ioport.p_api->pinWrite(LCD_CMD,IOPORT_LEVEL_HIGH);
	lcd_err = g_ioport.p_api->pinCfg(IOPORT_PORT_01_PIN_02,((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)((g_lcd_spi.p_cfg->clk_polarity == SPI_CLK_POLARITY_HIGH) ? IOPORT_LEVEL_LOW : IOPORT_LEVEL_HIGH)));
	R_BSP_SoftwareDelay(5,BSP_DELAY_UNITS_MICROSECONDS);
	lcd_err = g_ioport.p_api->pinCfg(IOPORT_PORT_01_PIN_02,((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI0_2_4_6_8));

	lcd_err = g_lcd_spi.p_api->writeRead(g_lcd_spi.p_ctrl, dummy_write, data,len,SPI_BIT_WIDTH_8_BITS);
	tx_semaphore_get(&g_lcd_spi_done,TX_WAIT_FOREVER);
	lcd_err = g_ioport.p_api->pinWrite(LCD_CS,IOPORT_LEVEL_HIGH);
}

void InitILI9341V(void)
{
	uint8_t data[8];
	int i;
	ssp_err_t lcd_err;
    /** Open the SPI driver to initialize the LCD **/
    g_lcd_spi.p_api->open(g_lcd_spi.p_ctrl, g_lcd_spi.p_cfg);

	/* Reset the ILI9341V */
	lcd_err = g_ioport.p_api->pinWrite(LCD_CS,IOPORT_LEVEL_HIGH);
	g_ioport.p_api->pinWrite(LCD_RESET,IOPORT_LEVEL_HIGH);
	g_ioport.p_api->pinWrite(LCD_RESET,IOPORT_LEVEL_LOW);
	tx_thread_sleep(1);
	g_ioport.p_api->pinWrite(LCD_RESET,IOPORT_LEVEL_HIGH);

	WriteLCD(ILI9341_SW_RESET, (uint8_t *)"\x0", 0);
	tx_thread_sleep(1);
	for (i=0; i<4; i++)
	{
		data[0] = (uint8_t)(0x10+i);
		WriteLCD(0xD9,data,1);
		ReadLCD(0xD3,data, 1);
	}

	WriteLCD(ILI9341_POWERB,	(uint8_t *)"\x00\xC1\x30", 3);
	WriteLCD(ILI9341_DTCA,		(uint8_t *)"\x85\x00\x78", 3);
	WriteLCD(ILI9341_DTCB,		(uint8_t *)"\x00\x00", 2);
	WriteLCD(ILI9341_POWERA,	(uint8_t *)"\x39\x2C\x00\x34\x02", 5);
	WriteLCD(ILI9341_POWER_SEQ, (uint8_t *)"\x64\x03\x12\x81", 4);
	WriteLCD(ILI9341_PRC,		(uint8_t *)"\x20", 1);
	WriteLCD(ILI9341_POWER1,	(uint8_t *)"\x23", 1);
	WriteLCD(ILI9341_POWER2,	(uint8_t *)"\x10", 1);
	WriteLCD(ILI9341_VCOM1,		(uint8_t *)"\x3E\x28", 2);
	WriteLCD(ILI9341_VCOM2,		(uint8_t *)"\x86", 1);
	WriteLCD(ILI9341_MAC,		(uint8_t *)"\x48", 1);
	WriteLCD(ILI9341_PIXEL_FORMAT,	(uint8_t *)"\x55", 1);  // DPI[101]: RGB 16 bits/pixel; DBI[101] MCU 16 bit/pixel
	WriteLCD(ILI9341_FRM_CTRL1,	(uint8_t *)"\x00\x18", 2);  // 79 khz
	WriteLCD(ILI9341_DFC,		(uint8_t *)"\x08\x82\x27", 3);
	WriteLCD(ILI9341_3GAMMA_EN,	(uint8_t *)"\x00", 1);
	WriteLCD(ILI9341_RGB_INTERFACE,	(uint8_t *)"\xC0", 1);  /* ByPass_Mode =1; RCM[1:0] = 10; VSPL=0; HSPL = 0; DPL=1; EPL=0 */
	WriteLCD(ILI9341_INTERFACE,	(uint8_t *)"\x01\x00\x06", 3); // DE pol - high enable, DOTCLK Pol = 1 data latch on falling, HSYNC Pol - High leveL Sync, Vsync Low level sync
	//WriteLCD(ILI9341_BPC,		"\x02\0x2\x0A\x14", 4); // Vert FP = 2 BP = 3; Horiz FP= 10, PB = 20
	WriteLCD(ILI9341_COLUMN_ADDR,	(uint8_t *)"\x00\x00\x00\xEF", 4);
	WriteLCD(ILI9341_PAGE_ADDR,	(uint8_t *)"\x00\x00\x01\x3F", 4);
	WriteLCD(ILI9341_GAMMA,	(uint8_t *)"\x01", 1);
	WriteLCD(ILI9341_PGAMMA,	(uint8_t *)"\x0F\x31\x2B\x0C\x0E\x08\x4E\xF1\x37\x07\x10\x03\x0E\x09\x00", 15);
	WriteLCD(ILI9341_NGAMMA,	(uint8_t *)"\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F", 15);
	WriteLCD(ILI9341_SLEEP_OUT,	(uint8_t *)"\x00", 1);
	tx_thread_sleep(2);
	WriteLCD(ILI9341_DISP_ON,	(uint8_t *)"\x00", 1);
	ReadLCD(ILI9341_READ_DISP_PIXEL,data,1);
}
