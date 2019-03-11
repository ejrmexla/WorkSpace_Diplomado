/* SystemThread entry function */
#include "system_thread.h"
#include "system_cfg.h"
#include "system_api.h"
#include "audio.h"
#if !defined(BSP_BOARD_S7G2_SK)
#include "brightness.h"
#endif
#include "volume_api.h"
#include "system_time.h"
#include "bsp_cfg.h"
#include "system_cfg.h"
#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"

#if defined(BSP_BOARD_S7G2_SK)
#include "guix_gen/lcd_setup/lcd.h"
#endif

void UsageFault_Handler(void);
void BusFault_Handler(void);
void Hard_Fault_Handler(uint32_t stack[]);
#if defined(__ICCARM__)
 __stackless void HardFault_Handler(void);
#elif defined(__GNUC__)
__attribute__((naked)) void HardFault_Handler(void);
#endif


GX_WINDOW*      p_splash_screen;
GX_WINDOW*      p_settings_screen;
GX_WINDOW*      p_thermostat_screen;
GX_WINDOW*      p_mainpage_screen;
GX_WINDOW*      p_help_screen;
GX_WINDOW_ROOT* p_root;

static system_state_t g_system_state =
{
    .time           = {0},
    .hour_mode      = SYSTEM_HOUR_MODE_12,
    .mode           = SYSTEM_MODE_OFF,
    .fan_mode       = SYSTEM_FAN_MODE_AUTO,
    .temp_units     = SYSTEM_TEMP_UNITS_C,
    .temp_c         = 22.0f,
    .target_temp    = 22.0f,
    .volume         = 10,
    .brightness     = 50
};

/** Post options are generally the same in this project, so store them once. */
const sf_message_post_cfg_t g_post_cfg =
{
    .priority = SF_MESSAGE_PRIORITY_NORMAL,
    .p_callback = NULL,
};

/** Acquire options are generally the same in this project, so store them once. */
const sf_message_acquire_cfg_t g_acquire_cfg =
{
    .buffer_keep = false,
};

volatile bool touched = false;

static system_payload_t             message_to_gx;
static temperature_payload_t*       p_temp_message;
static sf_touch_panel_payload_t*    p_touch_payload;

static void send_touch_message(sf_touch_panel_payload_t * p_payload);
static void send_hmi_message(gx_event_message_t event);

/***********************************************************************************************************************
 Private functions
 ***********************************************************************************************************************/

#if defined(BSP_BOARD_S7G2_SK)
void g_lcd_spi_callback(spi_callback_args_t * p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);
    tx_semaphore_ceiling_put (&g_lcd_spi_done, 1);
}
#endif

void system_thread_entry(void)
{
    ssp_err_t               err;
    UINT                    status = TX_SUCCESS;
    UINT                    gx_err = GX_SUCCESS;
    sf_message_header_t *   p_message = NULL;

#define INCLUDE_GUIX_CODE 1

#ifdef INCLUDE_GUIX_CODE

    /* Initializes GUIX. */
    gx_err = gx_system_initialize();
    if(TX_SUCCESS != gx_err)
    {
        while(1);
    }

    /* Initializes GUIX drivers. */
    gx_err = g_sf_el_gx.p_api->open (g_sf_el_gx.p_ctrl, g_sf_el_gx.p_cfg);
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }

    gx_err = gx_studio_display_configure ( DISPLAY,
                                           g_sf_el_gx.p_api->setup,
                                           LANGUAGE_ENGLISH,
                                           DISPLAY_THEME_1,
                                           &p_root );
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }

    err = g_sf_el_gx.p_api->canvasInit(g_sf_el_gx.p_ctrl, p_root);
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }

    /* Create the widgets we have defined with the GUIX data structures and resources. */
    gx_err = gx_studio_named_widget_create ("Splash", (GX_WIDGET *) p_root, (GX_WIDGET **) &p_splash_screen);
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }
    gx_err = gx_studio_named_widget_create ("Settings", GX_NULL, (GX_WIDGET **) &p_settings_screen);
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }
    gx_err = gx_studio_named_widget_create ("MainPage", GX_NULL, (GX_WIDGET **) &p_mainpage_screen);
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }
    gx_err = gx_studio_named_widget_create ("Thermostat", GX_NULL, (GX_WIDGET **) &p_thermostat_screen);
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }

#if !defined(BSP_BOARD_S7G2_SK)

    gx_err = gx_studio_named_widget_create ("Help", GX_NULL, (GX_WIDGET **) &p_help_screen);
    if(GX_SUCCESS != gx_err)
    {
        while(1);
    }
#endif
    /* Shows the root window to make it visible. */
    gx_err = gx_widget_show(p_root);
    if (GX_SUCCESS != gx_err)
    {
        while(1);
    }

    /* Lets GUIX run. */
    status = gx_system_start();
    if (GX_SUCCESS != gx_err)
    {
        while(1);
    }
#endif

#if !defined(BSP_BOARD_S7G2_SK)
    /* Controls the GPIO pin for LCD ON. */
    err = g_ioport.p_api->pinWrite(LCD_ON_PIN, IOPORT_LEVEL_HIGH);
    if (err)
    {
        while(1);
    }
#endif

#if BRIGHTNESS_DIMMING
    /* Opens PWM driver and controls the TFT panel back light. */
    err = g_pwm_backlight.p_api->open(g_pwm_backlight.p_ctrl, g_pwm_backlight.p_cfg);
    if (err)
    {
        while(1);
    }
#endif

    /* Initialize audio hardware and click data playback structure. */
#if AUDIO
    sf_audio_playback_data_t * p_click = NULL;
    audio_click_init(&p_click);
#endif

    /* Initialize time-keeping hardware. */
    system_time_init();

    /* Update time for the splash screen. */
    g_rtc.p_api->calendarTimeGet(g_rtc.p_ctrl, &g_system_state.time);

#if LCD_CONTROLLER_9341V
    InitILI9341V ();
#endif
    /* Create message in buffer. */
#ifdef INCLUDE_GUIX_CODE
    message_to_gx.gx_event.gx_event_type = GX_EVENT_PEN_DOWN;
    message_to_gx.state = g_system_state;
    message_to_gx.gx_event.gx_event_payload.gx_event_ulongdata = (ULONG) &message_to_gx;

    /* Post message. */
    gx_err =  gx_system_event_send(&message_to_gx.gx_event);
    if(GX_SUCCESS != status) {
        while(1);
    }
#endif


    while (1)
    {
        err = g_sf_message.p_api->pend (g_sf_message.p_ctrl, &system_thread_message_queue, (sf_message_header_t **) &p_message,
                                        TX_WAIT_FOREVER);
        if (SSP_SUCCESS != err) {
            while(1);
        }
#if AUDIO_IN_EACH_EVENT
        g_sf_audio_playback.p_api->start(g_sf_audio_playback.p_ctrl, &data, TX_NO_WAIT);
#endif
        switch (p_message->event_b.class_code)
        {
        case SF_MESSAGE_EVENT_CLASS_TEMPERATURE:
        {
            switch (p_message->event_b.code)
            {
            case SF_MESSAGE_EVENT_TEMPERATURE_INCREMENT:
                if (SYSTEM_MODE_OFF != g_system_state.mode)
                {
                    /** Increases target system temperature. */
                    g_system_state.target_temp += TEMP_INC_C;
                    if (TEMP_MAX_C < g_system_state.target_temp)
                    {
                        g_system_state.target_temp = TEMP_MAX_C;
                    }
                            send_hmi_message (GXEVENT_MSG_TEMPERATURE_INCREMENT);
                }
                break;
            case SF_MESSAGE_EVENT_TEMPERATURE_DECREMENT:
                if (SYSTEM_MODE_OFF != g_system_state.mode)
                {
                    /** Decreases target system temperature. */
                    g_system_state.target_temp -= TEMP_INC_C;
                    if (TEMP_MIN_C > g_system_state.target_temp)
                    {
                        g_system_state.target_temp = TEMP_MIN_C;
                    }
                            send_hmi_message (GXEVENT_MSG_TEMPERATURE_DECREMENT);
                }
                break;
            case SF_MESSAGE_EVENT_TEMPERATURE_UNIT_C:
                if (SYSTEM_TEMP_UNITS_C != g_system_state.temp_units)
                {
                    g_system_state.temp_units = SYSTEM_TEMP_UNITS_C;
                            send_hmi_message (GXEVENT_MSG_TEMPERATURE_UNIT_C);

                }
                break;
            case SF_MESSAGE_EVENT_TEMPERATURE_UNIT_F:
                if (SYSTEM_TEMP_UNITS_F != g_system_state.temp_units)
                {
                    g_system_state.temp_units = SYSTEM_TEMP_UNITS_F;
                    send_hmi_message (GXEVENT_MSG_TEMPERATURE_UNIT_F);
                }
                break;
            case SF_MESSAGE_EVENT_UPDATE_TEMPERATURE:
                p_temp_message = (temperature_payload_t *) p_message +1;
                g_system_state.temp_c = p_temp_message->temperature;
                        send_hmi_message (GXEVENT_MSG_UPDATE_TEMPERATURE);
                break;
            default:
                break;
            }
            break;
        }
        case SF_MESSAGE_EVENT_CLASS_SYSTEM:
            switch (p_message->event_b.code)
            {
            case SF_MESSAGE_EVENT_FAN_TOGGLE:
                /** Toggles fan setting. */
                g_system_state.fan_mode = (system_fan_mode_t)(!g_system_state.fan_mode);
                        send_hmi_message (GXEVENT_MSG_FAN_TOGGLE);
                break;
            case SF_MESSAGE_EVENT_SYSTEM_MODE_TOGGLE:
                /** Cycles between modes of system operation. */
                g_system_state.mode = (system_mode_t)((uint32_t)g_system_state.mode + 1) % (uint32_t)SYSTEM_MODE_MAX;
                        send_hmi_message (GXEVENT_MSG_SYSTEM_MODE_TOGGLE);
                break;
            default:
                break;
            }
            break;
        case SF_MESSAGE_EVENT_CLASS_DISPLAY:
        {
            switch (p_message->event_b.code)
            {
#if !defined(BSP_BOARD_S7G2_SK)

            case SF_MESSAGE_EVENT_BRIGHTNESS_INCREMENT:
                brightness_up(&g_system_state.brightness);
				send_hmi_message(GXEVENT_MSG_BRIGHTNESS_INCREMENT);
                break;
            case SF_MESSAGE_EVENT_BRIGHTNESS_DECREMENT:
                brightness_down(&g_system_state.brightness);
				send_hmi_message(GXEVENT_MSG_BRIGHTNESS_DECREMENT);
                break;
#endif
            default:
                break;
            }
            break;
        }
        case SF_MESSAGE_EVENT_CLASS_TIME:
        {
            switch (p_message->event_b.code)
            {
            case SF_MESSAGE_EVENT_HOUR_MODE_TOGGLE:
                g_system_state.hour_mode = (system_hour_mode_t)(!g_system_state.hour_mode);
                send_hmi_message (GXEVENT_MSG_TIME_UPDATE);
                break;
            case SF_MESSAGE_EVENT_SET_TIME:
            case SF_MESSAGE_EVENT_SET_DATE:
            {
                sf_message_payload_t * p_payload = (sf_message_payload_t *) (p_message+1);
                adjust_time(&p_payload->time_payload.time);
                break;
            }
            case SF_MESSAGE_EVENT_AM_PM_TOGGLE:
            {
                rtc_time_t time = {0};
                if (g_system_state.time.tm_hour < 12)
                {
                    time.tm_hour = 12;
                    adjust_time(&time);
                }
                else
                {
                    time.tm_hour = -12;
                    adjust_time(&time);
                }
                send_hmi_message (GXEVENT_MSG_AM_PM_TOGGLE);
                break;
            }
            case SF_MESSAGE_EVENT_UPDATE_TIME:
            {
                sf_message_payload_t * p_payload = (sf_message_payload_t *) (p_message+1);
				g_system_state.time = p_payload->time_payload.time;
              //  adjust_time(&p_payload->time_payload.time);
#ifdef INCLUDE_GUIX_CODE
                send_hmi_message (GXEVENT_MSG_TIME_UPDATE);
#endif
                break;
            }
            default:
                break;
            }
            break;
        }
#if AUDIO
        case SF_MESSAGE_EVENT_CLASS_VOLUME:
            switch (p_message->event_b.code)
            {
            case SF_MESSAGE_EVENT_VOLUME_INCREMENT:
                volume_up(&g_system_state.volume);
                            send_hmi_message (GXEVENT_MSG_VOLUME_INCREMENT);

                break;
            case SF_MESSAGE_EVENT_VOLUME_DECREMENT:
                volume_down(&g_system_state.volume);
                send_hmi_message (GXEVENT_MSG_VOLUME_DECREMENT);
                break;
            default:
                break;
            }
            break;
#endif
        case SF_MESSAGE_EVENT_CLASS_TOUCH:
        {
            switch (p_message->event_b.code)
            {
            case SF_MESSAGE_EVENT_NEW_DATA:
                //sf_touch_panel_payload_t * p_touch_payload = (sf_touch_panel_payload_t *) p_message;
                p_touch_payload = (sf_touch_panel_payload_t *) p_message;
                /** Translate a touch event into a GUIX event */
                send_touch_message(p_touch_payload);

                /** Play a click sound on touch down events. */
#if AUDIO
                if (SF_TOUCH_PANEL_EVENT_DOWN == p_touch_payload->event_type)
                {
                    err = g_sf_audio_playback.p_api->start(g_sf_audio_playback.p_ctrl, p_click, TX_NO_WAIT);
//                    if (SSP_SUCCESS != err)
//                    {
//                        while(1);
//                    }

                }
#endif
                touched = true;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }

        /** Message is processed, so release buffer. */
        err = g_sf_message.p_api->bufferRelease (g_sf_message.p_ctrl, (sf_message_header_t *) p_message,
                                                 SF_MESSAGE_RELEASE_OPTION_NONE);
        if (SSP_SUCCESS != err)
        {
            while(1);
        }


        /** Turn fan on or off based on fan mode, system mode, and target temperature. */
        if (SYSTEM_FAN_MODE_ON == g_system_state.fan_mode)
        {
            if (!g_system_state.fan_on)
            {
                g_system_state.fan_on = true;
                //Send message to update fan mode
                send_hmi_message(GXEVENT_MSG_FAN_TOGGLE);

            }
        }
        else
        {
            if (SYSTEM_MODE_OFF == g_system_state.mode)
            {
                if (g_system_state.fan_on)
                {
                    g_system_state.fan_on = false;
                    //Send message to update fan mode
                    send_hmi_message(GXEVENT_MSG_FAN_TOGGLE);

                }
            }
            else
            {
                if (SYSTEM_MODE_HEAT == g_system_state.mode)
                {
                    bool fan_on = g_system_state.temp_c < g_system_state.target_temp;

                    if (g_system_state.fan_on != fan_on)
                    {
                        g_system_state.fan_on = fan_on;
                        //Send message to update fan mode
                        send_hmi_message(GXEVENT_MSG_FAN_TOGGLE);

                    }
                }
                else
                {
                    bool fan_on = g_system_state.temp_c > g_system_state.target_temp;

                    if (g_system_state.fan_on != fan_on)
                    {
                        g_system_state.fan_on = fan_on;
                        //Send message to update fan mode
                        send_hmi_message(GXEVENT_MSG_FAN_TOGGLE);

                    }
                }
            }
        }
    }
}

static void send_hmi_message(gx_event_message_t event)
{
    UINT gx_err;


    /** Get mutex lock before accessing the pointer to the state data sent from the
     *  system thread, then save a local copy of the state data to be used by the GUI. **/
    tx_mutex_get(&g_state_data_mutex,TX_WAIT_FOREVER);

    /** Create message. */
    message_to_gx.gx_event.gx_event_type = event;
    message_to_gx.state = g_system_state;
    message_to_gx.gx_event.gx_event_target = GX_NULL;
    message_to_gx.gx_event.gx_event_payload.gx_event_ulongdata = (ULONG) &message_to_gx;

    /** Post message. */
    gx_err = gx_system_event_send (&message_to_gx.gx_event);
    if (GX_SUCCESS != gx_err)
    {
        while(1);
    }
    tx_mutex_put(&g_state_data_mutex);

}

/*******************************************************************************************************************//**
 * @brief  Sends a touch event to GUIX internal thread to call the GUIX event handler function
 *
 * @param[in] p_payload Touch panel message payload
***********************************************************************************************************************/
static void send_touch_message(sf_touch_panel_payload_t * p_payload)
{
    bool send_event = true;
    GX_EVENT gxe;
    UINT gx_status;

    switch (p_payload->event_type)
    {
    case SF_TOUCH_PANEL_EVENT_DOWN:
        gxe.gx_event_type = GX_EVENT_PEN_DOWN;
        break;
    case SF_TOUCH_PANEL_EVENT_UP:
        gxe.gx_event_type = GX_EVENT_PEN_UP;
        break;
    case SF_TOUCH_PANEL_EVENT_HOLD:
    case SF_TOUCH_PANEL_EVENT_MOVE:
        gxe.gx_event_type = GX_EVENT_PEN_DRAG;
        break;
    case SF_TOUCH_PANEL_EVENT_INVALID:
        send_event = false;
        break;
    default:
        break;
    }

    if (send_event)
    {
        /** Send event to GUI */
        gxe.gx_event_sender = GX_ID_NONE;
        gxe.gx_event_target = 0;
        gxe.gx_event_display_handle = 0;

        gxe.gx_event_payload.gx_event_pointdata.gx_point_x = p_payload->x;
#if defined(BSP_BOARD_S7G2_SK)
        /** Y coordinates must be swapped on S7G2 boards **/
        gxe.gx_event_payload.gx_event_pointdata.gx_point_y = (GX_VALUE)(320 - p_payload->y);
#else
        gxe.gx_event_payload.gx_event_pointdata.gx_point_y = p_payload->y;
#endif
        gx_status = gx_system_event_send (&gxe);
        if(GX_SUCCESS != gx_status) {
            while(1);
        }
    }
}

#if defined(__ICCARM__)
 __stackless void HardFault_Handler(void)
#elif defined(__GNUC__)
__attribute__((naked)) void HardFault_Handler(void)
#endif
{
    __asm volatile (
            "MRS r0, MSP   \n"
            "b Hard_Fault_Handler \n"
    );
}


void Hard_Fault_Handler(uint32_t stack[])
{
    SSP_PARAMETER_NOT_USED(stack);
//    volatile uint32_t stacked_r0;
//    volatile uint32_t stacked_r1;
//    volatile uint32_t stacked_r2;
//    volatile uint32_t stacked_r3;
//    volatile uint32_t stacked_r12;
//    volatile uint32_t stacked_lr;
//    volatile uint32_t stacked_pc;
//    volatile uint32_t stacked_psr;
//
//    static char msg[80];
//    sprintf (msg, "SCB->HFSR = 0x%08x\n", SCB->HFSR);
//    // Check to see if the forced bit is set in the HardFault Status Register
//    if ((SCB->HFSR & (1 << 30)) != 0)
//    {
//        //If the force bit is set read the CFSR (Configurable Fault Status Register)
//        sprintf (msg, "SCB->CFSR = 0x%08x\n", SCB->CFSR);
//        sprintf (msg, "SCB->BFAR = 0x%08x\n", SCB->BFAR);
//    }
//    stacked_r0 = ((uint32_t) stack[0]);
//    stacked_r1 = ((uint32_t) stack[1]);
//    stacked_r2 = ((uint32_t) stack[2]);
//    stacked_r3 = ((uint32_t) stack[3]);
//    stacked_r12 = ((uint32_t) stack[4]);
//    stacked_lr = ((uint32_t) stack[5]);
//    stacked_pc = ((uint32_t) stack[6]);
//    stacked_psr = ((uint32_t) stack[7]);

    while (1);
}

void BusFault_Handler(void)
{
    while (1);
}

void UsageFault_Handler(void)
{
    while (1);
}

