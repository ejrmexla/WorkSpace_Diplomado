/* generated thread header file - do not edit */
#ifndef SYSTEM_THREAD_H_
#define SYSTEM_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
extern "C" void system_thread_entry(void);
#else
extern void system_thread_entry(void);
#endif
#include "r_sci_spi.h"
#include "r_spi_api.h"
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "sf_external_irq.h"
#include "r_riic.h"
#include "r_i2c_api.h"
#include "sf_touch_panel_i2c.h"
#include "sf_touch_panel_api.h"
#include "r_rtc.h"
#include "r_rtc_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
extern const spi_cfg_t g_lcd_spi_cfg;
/** SPI on SCI Instance. */
extern const spi_instance_t g_lcd_spi;
extern sci_spi_instance_ctrl_t g_lcd_spi_ctrl;
extern const sci_spi_extended_cfg g_lcd_spi_cfg_extend;

#ifndef g_lcd_spi_callback
void g_lcd_spi_callback(spi_callback_args_t *p_args);
#endif

#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED)
#define g_lcd_spi_P_TRANSFER_TX (NULL)
#else
#define g_lcd_spi_P_TRANSFER_TX (&SYNERGY_NOT_DEFINED)
#endif
#if (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED)
#define g_lcd_spi_P_TRANSFER_RX (NULL)
#else
#define g_lcd_spi_P_TRANSFER_RX (&SYNERGY_NOT_DEFINED)
#endif
#undef SYNERGY_NOT_DEFINED

#define g_lcd_spi_P_EXTEND (&g_lcd_spi_cfg_extend)
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq;
#ifndef NULL
void NULL(external_irq_callback_args_t *p_args);
#endif
/** SF External IRQ on SF External IRQ Instance. */
extern const sf_external_irq_instance_t g_sf_external_irq;
extern const i2c_cfg_t g_i2c_cfg;
/** I2C on RIIC Instance. */
extern const i2c_master_instance_t g_i2c;
#ifndef NULL
void NULL(i2c_callback_args_t *p_args);
#endif

extern riic_instance_ctrl_t g_i2c_ctrl;
extern const riic_extended_cfg g_i2c_extend;
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED)
#define g_i2c_P_TRANSFER_TX (NULL)
#else
#define g_i2c_P_TRANSFER_TX (&SYNERGY_NOT_DEFINED)
#endif
#if (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED)
#define g_i2c_P_TRANSFER_RX (NULL)
#else
#define g_i2c_P_TRANSFER_RX (&SYNERGY_NOT_DEFINED)
#endif
#undef SYNERGY_NOT_DEFINED
#define g_i2c_P_EXTEND (&g_i2c_extend)
/** SF Touch Panel on SF Touch Panel I2C Instance. */
extern const sf_touch_panel_instance_t g_sf_touch_panel_i2c;
/** Messaging Framework Instance */
extern const sf_message_instance_t g_sf_message;
void g_sf_touch_panel_i2c_err_callback(void *p_instance, void *p_data);
void sf_touch_panel_i2c_init0(void);
/** RTC on RTC Instance. */
extern const rtc_instance_t g_rtc;
#ifndef time_update_callback
void time_update_callback(rtc_callback_args_t *p_args);
#endif
extern TX_QUEUE g_system_queue;
extern TX_SEMAPHORE g_lcd_spi_done;
extern TX_MUTEX g_state_data_mutex;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* SYSTEM_THREAD_H_ */
