/* This is a Weather demo event handler sample for GUIX graphics framework. */

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "gx_api.h"
#include "GlobalDefine.h"
#include "pwm_control.h"
#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"

#include "system_cfg.h"
#include "system_thread.h"
#include "system_api.h"

#include <stdio.h>
#include <math.h>

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
extern const sf_message_post_cfg_t g_post_cfg;
extern const sf_message_acquire_cfg_t g_acquire_cfg;

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
#if AUDIO_IN_EACH_EVENT
#   define CLICK_SOUND()  g_sf_audio_playback.p_api->start(g_sf_audio_playback.p_ctrl, &data, TX_NO_WAIT)
#else
#   define CLICK_SOUND()
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/** Application specific event codes to be used with GUIX. */
typedef enum e_app_event
{
    APP_EVENT_SYSTEM_DATA_UPDATE = GX_FIRST_APP_EVENT,
} app_event_t;

/***********************************************************************************************************************
Private global variables
***********************************************************************************************************************/
static USHORT cur_settings_screen = ID_SETTINGS_CONTENT_MENU;

static char g_local_temp_str [5]    = "00.0";
static char g_target_temp_str[5]    = "00.0";
static char g_time_str[9]           = "HH:MM AM";
static char g_hour_str[3]           = "HH";
static char g_min_str[3]            = "MM";
static char g_date_str[19]          = "September 00, 2000";
static char g_year_str[5]           = "2015";
static char g_day_str[3]            = "JAN";
static char g_bright_str[5]         = "100%";
static char g_vol_str[5]            = "100%";
static char g_wday_str[11]          = "Tuesday";


static GX_RESOURCE_ID const g_months[12] =
{
    GX_STRING_ID_M0, GX_STRING_ID_M1, GX_STRING_ID_M2, GX_STRING_ID_M3, GX_STRING_ID_M4, GX_STRING_ID_M5,
    GX_STRING_ID_M6, GX_STRING_ID_M7, GX_STRING_ID_M8, GX_STRING_ID_M9, GX_STRING_ID_M10, GX_STRING_ID_M11
};

static GX_RESOURCE_ID const g_months_short[12] =
{
    GX_STRING_ID_M0_SHORT, GX_STRING_ID_M1_SHORT, GX_STRING_ID_M2_SHORT, GX_STRING_ID_M3_SHORT,
    GX_STRING_ID_M4_SHORT, GX_STRING_ID_M5_SHORT, GX_STRING_ID_M6_SHORT, GX_STRING_ID_M7_SHORT,
    GX_STRING_ID_M8_SHORT, GX_STRING_ID_M9_SHORT, GX_STRING_ID_M10_SHORT, GX_STRING_ID_M11_SHORT
};

static GX_RESOURCE_ID const g_weekdays[7] =
{
    GX_STRING_ID_WD0, GX_STRING_ID_WD1, GX_STRING_ID_WD2, GX_STRING_ID_WD3,
    GX_STRING_ID_WD4, GX_STRING_ID_WD5, GX_STRING_ID_WD6
};

/** Stored GUI state. */
static system_state_t g_gui_state;

//sf_message_payload_t * p_time_message = NULL;


/***********************************************************************************************************************
Public global variables
***********************************************************************************************************************/
extern GX_WINDOW_ROOT * p_root;
extern GX_WINDOW   *p_splash_screen;
extern GX_WINDOW   *p_settings_screen;
extern GX_WINDOW   *p_thermostat_screen;
extern GX_WINDOW   *p_mainpage_screen;

#if AUDIO_IN_EACH_EVENT
#define AUDIO_CLICK_BUFFER_LENGTH (1414)

extern char click[];

extern sf_audio_playback_data_t data;
#endif


/***********************************************************************************************************************
Private function prototypes
***********************************************************************************************************************/
static float c_to_f(float temp);
static void get_message(sf_message_header_t ** pp_message, sf_message_payload_t ** pp_payload);
static void send_message(sf_message_header_t * p_message);
static void send_message_simple(uint8_t class, uint16_t code);
static void show_hide_widget(GX_WIDGET * p_widget, GX_RESOURCE_ID id, bool show);
static void update_text(GX_WIDGET * p_widget, GX_RESOURCE_ID id, char * p_text);
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id);
static void update_pixelmap_button_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT button);
static void update_time_screen(GX_WIDGET * p_widget, system_state_t * p_state);
static void update_date_screen(GX_WIDGET * p_widget, system_state_t * p_state);
static void update_time(GX_WIDGET *p_widget, system_state_t * p_state);
UINT settings_item_show(GX_WINDOW *widget, USHORT new_content, USHORT old_content);
static void update_state_data(GX_EVENT *event_ptr);
static void update_local_temp_string(void);
static void update_target_temp_string(void);
//static void settings_menu_highlight(GX_WINDOW *widget, USHORT menu_item);
static void toggle_screen(GX_WINDOW *new_win, GX_WINDOW *old_win);

/***********************************************************************************************************************
Public functions (Event Handlers)
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Handles all events on the splash screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT splashscreen_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;
    char * m;
    char * w;

    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_TIME_UPDATE:
            //This state is here to just insure the state data gets
            //updated while the splash screen is displayed so it will
            //be accurate when the main page gets displayed.
            update_state_data (event_ptr);

            //Write initial data/time to RTC when re-power cycle
            g_gui_state.time.tm_sec    = 0;
            g_gui_state.time.tm_min    = 0;
            g_gui_state.time.tm_hour   = 0;
            g_gui_state.time.tm_mday   = 1;
            g_gui_state.time.tm_wday   = 0;
            g_gui_state.time.tm_mon    = 0;
            g_gui_state.time.tm_year   = 100;
            g_rtc.p_api->calendarTimeSet(g_rtc.p_ctrl, &g_gui_state.time, true);

            update_time ((GX_WIDGET *) widget, &g_gui_state);
            break;

        //Can't catch GX_SIGNAL in splash screen so use pen down event
        case GX_EVENT_PEN_DOWN:
#if AUDIO_IN_EACH_EVENT
            g_sf_audio_playback.p_api->start(g_sf_audio_playback.p_ctrl, &data, TX_NO_WAIT);
#endif
            gx_system_timer_stop(widget, 10);
            toggle_screen(p_mainpage_screen,p_splash_screen);
        break;
    case GX_EVENT_TIMER:
            gx_system_timer_stop(widget, 10);
            toggle_screen(p_mainpage_screen,p_splash_screen);
            break;
        case GX_EVENT_SHOW:
            gx_system_timer_start(widget, 10 , SPLASH_TIMEOUT, SPLASH_TIMEOUT);
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                while(1);
            }

            //Write initial data/time to RTC when re-power cycle
            g_gui_state.time.tm_sec    = 0;
            g_gui_state.time.tm_min    = 0;
            g_gui_state.time.tm_hour   = 0;
            g_gui_state.time.tm_mday   = 1;
            g_gui_state.time.tm_wday   = 0;
            g_gui_state.time.tm_mon    = 0;
            g_gui_state.time.tm_year   = 100;
            g_rtc.p_api->calendarTimeSet(g_rtc.p_ctrl, &g_gui_state.time, true);

           /** Update time string. */
            update_time((GX_WIDGET *)widget, &g_gui_state);

            /** Update the date text on the splash screen. */
            gx_err = gx_system_string_get(g_months[g_gui_state.time.tm_mon], &m);
            if(GX_SUCCESS != gx_err) {
                while(1);
            }
            snprintf(g_date_str, sizeof(g_date_str), "%s %d, %d", m, g_gui_state.time.tm_mday, g_gui_state.time.tm_year + 1900);
            update_text((GX_WIDGET *) widget, ID_DATE_TEXT, g_date_str);

            /** Update the date text on the splash screen. */
            gx_err = gx_system_string_get(g_weekdays[g_gui_state.time.tm_wday], &w);
            if(GX_SUCCESS != gx_err) {
                while(1);
            }
            /** Update the date text on the splash screen. */
            snprintf(g_wday_str, sizeof(g_wday_str), "%s", w);
            update_text((GX_WIDGET *) widget, ID_WEEKDAY_TEXT, g_wday_str);
            break;
        default:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                while(1);
            }
            break;
    }

    return gx_err;
}

/*******************************************************************************************************************//**
 * @brief   Handles all events on the main screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT mainpage_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;

    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_UPDATE_TEMPERATURE:
            /** Update temperature text. */
            update_state_data(event_ptr);
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            pwm_control_entry ();
            update_text((GX_WIDGET *) widget, ID_DUTY_CICLE, g_dutycicle_str);
            update_text((GX_WIDGET *) widget, ID_PORCENT_CICLE, g_porcent_str);
            //update_text((GX_WIDGET *) widget, ID_TEMP_TEXT_2, g_local_temp_str);
            break;

        case GXEVENT_MSG_TIME_UPDATE:
            update_state_data (event_ptr);
            update_time ((GX_WIDGET *) widget, &g_gui_state);
            break;
        case GXEVENT_MSG_FAN_TOGGLE:
            update_state_data(event_ptr);
            if (g_gui_state.fan_on)
            {
                show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, 1);
            }
            else
            {
                show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, 0);
            }
            break;
        case GX_SIGNAL(ID_THERMO_BUTTON, GX_EVENT_CLICKED):
                CLICK_SOUND();
                /** Shows the thermostat control screen. */
                toggle_screen (p_mainpage_screen, widget);
            break;
        case GX_SIGNAL(ID_SETTINGS_BUTTON, GX_EVENT_CLICKED):
                CLICK_SOUND();
                /** Shows the settings screen and saves which screen the user is currently viewing. */
                toggle_screen (p_settings_screen, widget);
            break;
        case GX_SIGNAL(ID_START_BUTTON, GX_EVENT_CLICKED):
            g_ioport.p_api->pinWrite(IOPORT_PORT_00_PIN_10, IOPORT_LEVEL_HIGH);
            break;
        case GX_SIGNAL(ID_STOP_BUTTON, GX_EVENT_CLICKED):
            g_ioport.p_api->pinWrite(IOPORT_PORT_00_PIN_10, IOPORT_LEVEL_LOW);
            break;
        case GX_EVENT_SHOW:
            /** Update initial text fields according to system settings before the window shows. */
            /** Do default window processing first. */
            gx_err = gx_window_event_process (widget, event_ptr);
            if (GX_SUCCESS != gx_err) {
                while(1);
            }

            /** Update time string on status bar. */
            update_time((GX_WIDGET *)widget, &g_gui_state);

            /** Set the initial temperature units. */
            if(SYSTEM_TEMP_UNITS_F == g_gui_state.temp_units) {
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_FAHRENHEIT);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_FAHRENHEIT);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_2, GX_STRING_ID_FAHRENHEIT);
            } else {
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_CELSIUS);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_CELSIUS);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_2, GX_STRING_ID_CELSIUS);
            }

            /** Update the local temperature string in the status bar. */
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT_2, g_local_temp_str);

            /** Set the initial system mode text. */
            switch (g_gui_state.mode)
            {
                case SYSTEM_MODE_OFF:
                    update_text_id((GX_WIDGET *) widget, ID_SYSTEM_MODE_TEXT, GX_STRING_ID_SYSTEM_MODE_OFF);
                    break;
                case SYSTEM_MODE_COOL:
                    update_text_id((GX_WIDGET *) widget, ID_SYSTEM_MODE_TEXT, GX_STRING_ID_SYSTEM_MODE_COOL);
                    break;
                case SYSTEM_MODE_HEAT:
                    update_text_id((GX_WIDGET *) widget, ID_SYSTEM_MODE_TEXT, GX_STRING_ID_SYSTEM_MODE_HEAT);
                    break;
                default:
                    break;
            }

            /** Show/Hide fan icon. */
            show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, g_gui_state.fan_on);

            /** Set the initial fan mode text. */
            switch (g_gui_state.fan_mode)
            {
                case SYSTEM_FAN_MODE_AUTO:
                    update_text_id((GX_WIDGET *) widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_AUTO);
                    break;
                case SYSTEM_FAN_MODE_ON:
                    update_text_id((GX_WIDGET *) widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_ON);
                    break;
                default:
                    break;
            }

            /** Update "set to" temperature text. */
            if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
            {
                int integer = (int)floorf(g_gui_state.target_temp);
                int frac    = (int)roundf((g_gui_state.target_temp - (float)integer) * 10.0f);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%2d.%1d", integer, frac);
            }
            else
            {
                float temp_f = c_to_f(g_gui_state.target_temp);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%d", (int) temp_f);
            }
            update_text((GX_WIDGET *) widget, ID_SET_TO_TEXT, g_target_temp_str);
            break;
    default:
            gx_err = gx_window_event_process (widget, event_ptr);
            if (GX_SUCCESS != gx_err) {
                while(1);
            }
        break;
    }

    return gx_err;
}

/*******************************************************************************************************************//**
 * @brief   Handles all events on the thermostat screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT thermostat_screen_event(GX_WINDOW * p_window, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;
    GX_WIDGET * p_widget = (GX_WIDGET *) p_window;
    GX_RESOURCE_ID mode_text = GX_STRING_ID_SYSTEM_MODE_OFF;
    bool system_off_text = true;

    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_UPDATE_TEMPERATURE:
            /** Update temperature text. */
            update_state_data(event_ptr);
            update_local_temp_string();
            update_text((GX_WIDGET *) p_widget, ID_TEMP_TEXT, g_local_temp_str);
            update_text((GX_WIDGET *) p_widget, ID_DUTY_CICLE, g_dutycicle_str);
            break;

        case GXEVENT_MSG_TIME_UPDATE:
            update_state_data(event_ptr);
            update_time(p_widget, &g_gui_state);
            break;
        case GXEVENT_MSG_TEMPERATURE_INCREMENT:
        case GXEVENT_MSG_TEMPERATURE_DECREMENT:
        {
            update_state_data(event_ptr);
            /** Update "set to" temperature text. */
            if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
            {
                int integer = (int)floorf(g_gui_state.target_temp);
                int frac    = (int)roundf((g_gui_state.target_temp - (float)integer) * 10.0f);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%2d.%1d", integer, frac);
            }
            else
            {
                float temp_f = c_to_f(g_gui_state.target_temp);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%d", (int) temp_f);
            }
            update_text((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, g_target_temp_str);
        }
            break;
        case GXEVENT_MSG_FAN_TOGGLE:
            update_state_data(event_ptr);
            if (SYSTEM_FAN_MODE_AUTO == g_gui_state.fan_mode)
            {
                update_pixelmap_button_id((GX_WIDGET *) p_widget, ID_FAN_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
                update_text_id((GX_WIDGET *) p_widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_AUTO);
            }
            else
            {
                update_pixelmap_button_id((GX_WIDGET *) p_widget, ID_FAN_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
                update_text_id((GX_WIDGET *) p_widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_ON);
            }
            if (g_gui_state.fan_on)
            {
                show_hide_widget((GX_WIDGET *) p_widget, ID_FAN_ICON, 1);
            }
            else
            {
                show_hide_widget((GX_WIDGET *) p_widget, ID_FAN_ICON, 0);
            }
            break;
        case GXEVENT_MSG_SYSTEM_MODE_TOGGLE:
            update_state_data(event_ptr);
            /** Update system mode button. */
            switch (g_gui_state.mode)
            {
                case SYSTEM_MODE_OFF:
                    update_pixelmap_button_id ((GX_WIDGET *) p_widget, ID_SYSTEM_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
                    mode_text = GX_STRING_ID_SYSTEM_MODE_OFF;
                    system_off_text = true;
                    break;
                case SYSTEM_MODE_COOL:
                    update_pixelmap_button_id ((GX_WIDGET *) p_widget, ID_SYSTEM_BUTTON, GX_PIXELMAP_ID_BLUEBUTTON);
                    mode_text = GX_STRING_ID_SYSTEM_MODE_COOL;
                    system_off_text = false;
                    break;
                case SYSTEM_MODE_HEAT:
                    update_pixelmap_button_id ((GX_WIDGET *) p_widget, ID_SYSTEM_BUTTON, GX_PIXELMAP_ID_REDBUTTON);
                    mode_text = GX_STRING_ID_SYSTEM_MODE_HEAT;
                    system_off_text = false;
                    break;
                default:
                    break;
            }
            /** Update text fields related to system setting. */
            update_text_id((GX_WIDGET *) p_widget, ID_SYSTEM_MODE_TEXT, mode_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_SYSTEM_OFF, system_off_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, !system_off_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_1, !system_off_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_TARGET_TEMP_DEGREE_TEXT, !system_off_text);
            break;
    case GX_SIGNAL(ID_SETTINGS_BUTTON, GX_EVENT_CLICKED):
#if AUDIO_IN_EACH_EVENT
        g_sf_audio_playback.p_api->start(g_sf_audio_playback.p_ctrl, &data, TX_NO_WAIT);
#endif
        /** Shows the settings screen and saves which screen the user is currently viewing. */
            toggle_screen(p_settings_screen,p_mainpage_screen);//p_thermostat_screen
        break;
        case GX_SIGNAL(ID_THERMOSTAT_BACK_BUTTON, GX_EVENT_CLICKED):
#if AUDIO_IN_EACH_EVENT
        g_sf_audio_playback.p_api->start(g_sf_audio_playback.p_ctrl, &data, TX_NO_WAIT);
#endif
        /** Returns to main screen. */
            toggle_screen(p_mainpage_screen,p_settings_screen);//p_thermostat_screen
        break;
    case GX_SIGNAL(ID_TEMP_UP, GX_EVENT_CLICKED):
        /** Create temp up message. */
        send_message_simple(SF_MESSAGE_EVENT_CLASS_TEMPERATURE, SF_MESSAGE_EVENT_TEMPERATURE_INCREMENT);
        break;
    case GX_SIGNAL(ID_TEMP_DOWN, GX_EVENT_CLICKED):
        /** Create temp down message. */
        send_message_simple(SF_MESSAGE_EVENT_CLASS_TEMPERATURE, SF_MESSAGE_EVENT_TEMPERATURE_DECREMENT);
        break;
    case GX_SIGNAL(ID_FAN_BUTTON, GX_EVENT_CLICKED):
        /** Create fan toggle message. */
        send_message_simple(SF_MESSAGE_EVENT_CLASS_SYSTEM, SF_MESSAGE_EVENT_FAN_TOGGLE);
        break;
    case GX_SIGNAL(ID_SYSTEM_BUTTON, GX_EVENT_CLICKED):
        /** Create thermostat toggle message. */
        send_message_simple(SF_MESSAGE_EVENT_CLASS_SYSTEM, SF_MESSAGE_EVENT_SYSTEM_MODE_TOGGLE);
        break;
    case GX_EVENT_SHOW:
        /** Continue with default window processing. */
        gx_err = gx_window_event_process (p_window, event_ptr);
        if (GX_SUCCESS != gx_err) {
            while(1);
        }

        /** Update time string on status bar. */
        update_time(p_widget, &g_gui_state);

        //update_date_screen(p_widget, &g_gui_state);
//        update_date(p_widget, &g_gui_state);

        /** Update target (set to) text. */
        update_target_temp_string();
        update_text((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, g_target_temp_str);

        /** Update local temperature text. */
        update_local_temp_string();
        update_text((GX_WIDGET *) p_widget, ID_TEMP_TEXT, g_local_temp_str);
        update_text((GX_WIDGET *) p_widget, ID_DUTY_CICLE, g_dutycicle_str);
        /** Set the temperature units according to the current settings. */
        if(SYSTEM_TEMP_UNITS_F == g_gui_state.temp_units) {
            update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_FAHRENHEIT);
            update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_FAHRENHEIT);
        } else {
           update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_CELSIUS);
           update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_CELSIUS);
        }

        /** Show hide widgets based on the system mode. */
        show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_SYSTEM_OFF, (bool) !g_gui_state.mode);
        show_hide_widget((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, (bool) g_gui_state.mode);
        show_hide_widget((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_1, (bool) g_gui_state.mode);
        show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_TARGET_TEMP_DEGREE_TEXT, (bool) g_gui_state.mode);

        /** Show/Hide fan icon. */
        show_hide_widget((GX_WIDGET *) p_widget, ID_FAN_ICON, g_gui_state.fan_on);
        break;
    default:
            gx_err = gx_window_event_process (p_window, event_ptr);
            if (GX_SUCCESS != gx_err) {
                while(1);
            }
        break;
    }

    return gx_err;
}

/*******************************************************************************************************************//**
 * @brief   Sends messages corresponding to the button touched.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT settings_screen_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{

    sf_message_header_t * p_message;
    sf_message_payload_t * p_payload;

    UINT gx_err = GX_SUCCESS;
    gx_err = gx_window_event_process (widget, event_ptr);
    if (GX_SUCCESS != gx_err) {
        while(1);
    }

    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_UPDATE_TEMPERATURE:
            update_state_data(event_ptr);
            /** Update temperature text. */
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            //update_text((GX_WIDGET *) widget, ID_TEMP_TEXT_2, g_local_temp_str);
            update_text((GX_WIDGET *) widget, ID_DUTY_CICLE, g_dutycicle_str);
            break;
        case GXEVENT_MSG_TIME_UPDATE:
            update_state_data(event_ptr);
            /** Update time on status bar. */
            update_time((GX_WIDGET *) widget, &g_gui_state);
            /** Update time or date screen depending on which screen we're in. */
            switch(cur_settings_screen) {
                case ID_SETTINGS_CONTENT_DATE:
                    update_date_screen((GX_WIDGET *) widget, &g_gui_state);
//                    update_date((GX_WIDGET *) widget, &g_gui_state);
                    break;
                case ID_SETTINGS_CONTENT_TIME:
                    update_time_screen((GX_WIDGET *) widget, &g_gui_state);
                    break;
            }
            break;
        case GXEVENT_MSG_TEMPERATURE_UNIT_C:
            update_state_data(event_ptr);
            /** Update temperature unit. */
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
            update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_CELSIUS);
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);

            break;
        case GXEVENT_MSG_TEMPERATURE_UNIT_F:
            update_state_data(event_ptr);
            /** Update temperature unit. */
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
            update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_FAHRENHEIT);
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            break;
        case GXEVENT_MSG_VOLUME_INCREMENT:
        case GXEVENT_MSG_VOLUME_DECREMENT:
            update_state_data(event_ptr);
            /** Update volume text. */
            snprintf(g_vol_str, sizeof(g_vol_str), "%d%%", g_gui_state.volume);
            update_text((GX_WIDGET *) widget, ID_VOLUME_TEXT, g_vol_str);
            break;
        case GXEVENT_MSG_FAN_TOGGLE:
            update_state_data(event_ptr);
            if (g_gui_state.fan_on)
            {
                show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, 1);
            }
            else
            {
                show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, 0);
            }
            break;
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_THERMOSTAT, GX_EVENT_CLICKED):
            settings_item_show (widget, ID_SETTINGS_CONTENT_THERMO, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_THERMO;
            break;
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_TIME, GX_EVENT_CLICKED):
            update_time_screen((GX_WIDGET *) widget, &g_gui_state);
            settings_item_show (widget, ID_SETTINGS_CONTENT_TIME, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_TIME;
            break;
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_DATE, GX_EVENT_CLICKED):
            update_date_screen((GX_WIDGET *) widget, &g_gui_state);
            settings_item_show (widget, ID_SETTINGS_CONTENT_DATE, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_DATE;
            break;

            /*if (ID_SETTINGS_CONTENT_MENU == cur_settings_screen)
            {
                toggle_screen (p_mainpage_screen, widget);
            }
            else
            {
                settings_item_show (widget, ID_SETTINGS_CONTENT_MENU, cur_settings_screen);
                cur_settings_screen = ID_SETTINGS_CONTENT_MENU;
            }*/
        case GX_SIGNAL(ID_BACK_BUTTON, GX_EVENT_CLICKED):
        case GX_SIGNAL(ID_SETTINGS_BACK_BUTTON, GX_EVENT_CLICKED):
            toggle_screen (p_mainpage_screen, widget);
            break;
        case GX_SIGNAL(ID_CELSIUS_BUTTON, GX_EVENT_CLICKED):
            /** Create message to set temperature units to Celsius. */
            send_message_simple(SF_MESSAGE_EVENT_CLASS_TEMPERATURE, SF_MESSAGE_EVENT_TEMPERATURE_UNIT_C);
            break;
        case GX_SIGNAL(ID_FAHRENHEIT_BUTTON, GX_EVENT_CLICKED):
            /** Create message to set temperature units to Fahrenheit. */
            send_message_simple(SF_MESSAGE_EVENT_CLASS_TEMPERATURE, SF_MESSAGE_EVENT_TEMPERATURE_UNIT_F);
            break;
        case GX_SIGNAL(ID_HOUR_MODE_TOGGLE, GX_EVENT_CLICKED):
            /** Create message to toggle hour mode. */
            send_message_simple(SF_MESSAGE_EVENT_CLASS_TIME, SF_MESSAGE_EVENT_HOUR_MODE_TOGGLE);
            break;
        case GX_SIGNAL(ID_HOUR_UP, GX_EVENT_CLICKED):
            /** Create message to set hour. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            p_payload->time_payload.time.tm_hour = 1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_HOUR_DOWN, GX_EVENT_CLICKED):
            /** Create message to set hour. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            p_payload->time_payload.time.tm_hour = -1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MINUTE_UP, GX_EVENT_CLICKED):
            /** Create message to set minute. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            p_payload->time_payload.time.tm_min = 1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MINUTE_DOWN, GX_EVENT_CLICKED):
            /** Create message to set minute. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            p_payload->time_payload.time.tm_min = -1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_YEAR_UP, GX_EVENT_CLICKED):
            /** Create message to set year. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            p_payload->time_payload.time.tm_year = 1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_YEAR_DOWN, GX_EVENT_CLICKED):
            /** Create message to set year. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            p_payload->time_payload.time.tm_year = -1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MONTH_UP, GX_EVENT_CLICKED):
            /** Create message to set month. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            p_payload->time_payload.time.tm_mon = 1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MONTH_DOWN, GX_EVENT_CLICKED):
            /** Create message to set month. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            p_payload->time_payload.time.tm_mon = -1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_DAY_UP, GX_EVENT_CLICKED):
            /** Create message to set day. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            p_payload->time_payload.time.tm_mday = 1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_DAY_DOWN, GX_EVENT_CLICKED):
            /** Create message to set day. */
            get_message(&p_message, &p_payload);
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            p_payload->time_payload.time.tm_mday = -1;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_VOLUME_UP, GX_EVENT_CLICKED):
            /** Create message to set volume. */
            send_message_simple(SF_MESSAGE_EVENT_CLASS_VOLUME, SF_MESSAGE_EVENT_VOLUME_INCREMENT);
            break;
        case GX_SIGNAL(ID_VOLUME_DOWN, GX_EVENT_CLICKED):
            /** Create message to set volume. */
            send_message_simple(SF_MESSAGE_EVENT_CLASS_VOLUME, SF_MESSAGE_EVENT_VOLUME_DECREMENT);
            break;
        case GX_EVENT_SHOW:
            gx_err = gx_window_event_process (widget, event_ptr);
            if (GX_SUCCESS != gx_err) {
                while(1);
            }
            /** Update time and date strings on status bar */
            update_time((GX_WIDGET *) widget, &g_gui_state);

            /** Show/Hide fan icon. */
            show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, g_gui_state.fan_on);

            if(SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units) {
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
            } else {
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
            }

            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);

            snprintf(g_vol_str, sizeof(g_vol_str), "%d%%", g_gui_state.volume);
            update_text((GX_WIDGET *) widget, ID_VOLUME_TEXT, g_vol_str);
            pwm_control_entry ();
            snprintf(g_dutycicle_str, sizeof(g_dutycicle_str), "%d", display_dutycycle);
            update_text((GX_WIDGET *) widget, ID_DUTY_CICLE, g_dutycicle_str);

            /** Update brightness text. */
            snprintf(g_bright_str, sizeof(g_bright_str), "%d%%", g_gui_state.brightness);
            update_text((GX_WIDGET *) widget, ID_BRIGHTNESS_TEXT, g_bright_str);
        break;
    default:
        gx_err = gx_window_event_process (widget, event_ptr);
        if (GX_SUCCESS != gx_err) {
            while(1);
        }
        break;
    }

    return gx_err;
}


/***********************************************************************************************************************
Private Functions
***********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief   Converts a Celsius temperature to a Fahrenheit temperature.
 *
 * @param[in]   temp        Celsius temperature to convert
 *
 * @returns The converted temperature
***********************************************************************************************************************/
static float c_to_f(float temp)
{
    return (temp * 9.0f/5.0f) + 32.0f;
}


/*******************************************************************************************************************//**
 * @brief   Helper function to acquire message.
 *
 * @param[out]   pp_message     Pointer to pointer to message buffer to use
 * @param[out]   pp_payload     Pointer to pointer to start of payload in the buffer
***********************************************************************************************************************/
static void get_message(sf_message_header_t ** pp_message, sf_message_payload_t ** pp_payload)
{
    /** Get buffer from messaging framework. */
    sf_message_header_t * p_message = NULL;
    ssp_err_t err;
    err =  g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl, (sf_message_header_t **) &p_message,
            &g_acquire_cfg, TX_NO_WAIT);
    if (SSP_SUCCESS != err)
    {
        while(1);
    }

    /** Set return parameters. */
    *pp_message = p_message;
    *pp_payload = (void*)(p_message+1);

    /** Initialize payload. */
    memset(*pp_payload, 0, sizeof(sf_message_payload_t));
}

/*******************************************************************************************************************//**
 * @brief   Helper function to post message.
 *
 * @param[out]   p_message     Pointer to message to send.
***********************************************************************************************************************/
static void send_message(sf_message_header_t * p_message)
{
    ssp_err_t err;

    /** Post message. */
    sf_message_post_err_t post_err;
    err =  g_sf_message.p_api->post(g_sf_message.p_ctrl, (sf_message_header_t *) p_message, &g_post_cfg, &post_err, TX_NO_WAIT);
    if (SSP_SUCCESS != err)
    {
        while(1);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to post a message which doesn't have a payload.
 *
 * @param[out]   p_message     Pointer to message to send.
***********************************************************************************************************************/
static void send_message_simple(uint8_t class, uint16_t code) {
    sf_message_header_t * p_message = NULL;
    sf_message_payload_t * p_unused  = NULL;
    get_message(&p_message, &p_unused);

    p_message->event_b.class_code = class;
    p_message->event_b.code  = code;

    send_message(p_message);
}

/*******************************************************************************************************************//**
 * @brief   Helper function to show or hide widgets based on ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   show        Whether to show (true) or hide (false) the widget
***********************************************************************************************************************/
static void show_hide_widget(GX_WIDGET * p_widget, GX_RESOURCE_ID id, bool show)
{
    GX_WIDGET * p_target_widget;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_target_widget);
    if (GX_SUCCESS == err)
    {
        if (show)
        {
            err = gx_widget_show(p_target_widget);
            if (GX_SUCCESS != err) {
                while(1);
            }

        }
        else
        {
            err = gx_widget_hide(p_target_widget);
            if (GX_SUCCESS != err) {
                while(1);
            }

        }
    } else {
        while(1);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to update strings based on resource ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   p_text      Pointer to string to display
***********************************************************************************************************************/
static void update_text(GX_WIDGET * p_widget, GX_RESOURCE_ID id, char * p_text)
{
    GX_PROMPT * p_prompt = NULL;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_prompt);
    if (GX_SUCCESS == err)
    {
        err = gx_prompt_text_set(p_prompt, p_text);
        if (GX_SUCCESS != err) {
            while(1);
        }

    } else {
        while(1);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to update strings based on ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   string_id   ID of string to display
***********************************************************************************************************************/
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id)
{
    GX_PROMPT * p_prompt = NULL;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_prompt);
    if (GX_SUCCESS == err)
    {
        err = gx_prompt_text_id_set(p_prompt, string_id);
        if (GX_SUCCESS != err) {
            while(1);
        }
    } else {
        while(1);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to update pixelmap buttons based on ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   button      ID of button to display
***********************************************************************************************************************/
static void update_pixelmap_button_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT button)
{
    GX_PIXELMAP_BUTTON * p_button = NULL;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_button);
    if (GX_SUCCESS == err)
    {
        err = gx_pixelmap_button_pixelmap_set(p_button, button, button, button);
        if (GX_SUCCESS != err) {
            while(1);
        }

    } else {
        while(1);
    }
}
/*******************************************************************************************************************//**
 * @brief   Helper function to copy state data from pointer provided by system thread. Obtains a mutex lock before the
 *          copying the data.
 *
 * @param[in]   *event_ptr  Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   button      ID of button to display
***********************************************************************************************************************/
static void update_state_data(GX_EVENT *event_ptr) {

    system_payload_t * p_payload;

    /** Get mutex lock before accessing the pointer to the state data sent from the
     *  system thread, then save a local copy of the state data to be used by the GUI. **/
    tx_mutex_get(&g_state_data_mutex,TX_WAIT_FOREVER);

    p_payload = (system_payload_t *) event_ptr->gx_event_payload.gx_event_ulongdata;
    g_gui_state = p_payload->state;

    tx_mutex_put(&g_state_data_mutex);

}

/*******************************************************************************************************************//**
 * @brief   Updates the text on the widget to reflect the current time (in the format HH:MM [AM/PM]).
 *
 * @param[in]   p_widget    Pointer to the widget that caused the event
 * @param[in]   p_state     Current state received from system monitor
***********************************************************************************************************************/
static void update_time(GX_WIDGET *p_widget, system_state_t * p_state)
{
    int h;
    int m;
    char * p = "";
    char const * fmt = "%02d:%02d";
    h = p_state->time.tm_hour;
    m = p_state->time.tm_min;
    UINT gx_err;

    if (SYSTEM_HOUR_MODE_12 == p_state->hour_mode)
    {
        fmt = "%d:%02d %s";
        if (h < 12)
        {
            gx_err = gx_system_string_get(GX_STRING_ID_PERIOD_AM, &p);
            if (GX_SUCCESS != gx_err)
            {
                while(1);
            }
        }
        else
        {
            h -= 12;
            gx_err = gx_system_string_get(GX_STRING_ID_PERIOD_PM, &p);
            if (GX_SUCCESS != gx_err)
            {
                while(1);
            }
        }

        if (h == 0)
        {
            h = 12;
        }
    }
    snprintf(g_time_str, sizeof(g_time_str), fmt, h, m, p);
    update_text(p_widget, ID_TIME_TEXT, g_time_str);

}

/*******************************************************************************************************************//**
 * @brief   Updates the text on all time and date related widgets.
 *
 * @param[in]   p_widget    Pointer to the parent widget to search
 * @param[in]   p_state     Current state received from system monitor
***********************************************************************************************************************/
static void update_date_screen(GX_WIDGET *p_widget, system_state_t * p_state)
{
    /** Update date string. */

    /** Update year text. */
	/** Year is changed to 1900 from 2000, this is because driver will add 100 to it */
	/** Above modification are made w.r.t to c time.h standard */
    snprintf(g_year_str, sizeof(g_year_str), "%d", p_state->time.tm_year + 1900);
    update_text(p_widget, ID_YEAR_TEXT, g_year_str);

    /** Update month text. */
    update_text_id(p_widget, ID_MONTH_TEXT, g_months_short[p_state->time.tm_mon]);

    /** Update day text. */
    snprintf(g_day_str, sizeof(g_day_str), "%d", p_state->time.tm_mday);
    update_text(p_widget, ID_DAY_TEXT, g_day_str);

}

/*******************************************************************************************************************//**
 * @brief   Updates the text on all time and date related widgets.
 *
 * @param[in]   p_widget    Pointer to the parent widget to search
 * @param[in]   p_state     Current state received from system monitor
***********************************************************************************************************************/
static void update_time_screen(GX_WIDGET *p_widget, system_state_t * p_state)
{
    /** Update hour text. */
    char const * fmt = "%02ld";
    int h = p_state->time.tm_hour;

    UINT hour_mode_string = GX_STRING_ID_FORMAT_24;
    if (SYSTEM_HOUR_MODE_12 == p_state->hour_mode)
    {
        hour_mode_string = GX_STRING_ID_FORMAT_12;
        fmt = "%d";
        if (h > 12)
        {
            h -= 12;
        }
        if (h == 0)
        {
            h = 12;
        }
    }
    /** Update hour text */
    snprintf(g_hour_str, sizeof(g_hour_str), fmt, h);
    update_text(p_widget, ID_HOUR_TEXT, g_hour_str);
    update_text_id(p_widget, ID_TIME_FORMAT_TEXT, hour_mode_string);

    /** Update minute text. */
    snprintf(g_min_str, sizeof(g_min_str), "%02d", p_state->time.tm_min);
    update_text(p_widget, ID_MINUTE_TEXT, g_min_str);

    /** Update time string. */
    update_time(p_widget, p_state);
}


/*******************************************************************************************************************//**
 * @brief   Toggles between top level screens defined with .
 *
 * @param[in]   new_win  The pointer to the window to show
 * @param[in]   old_win  The pinter to the old window to hide
 *
 * @retval      GX_SUCCESS
 ***********************************************************************************************************************/
void toggle_screen(GX_WINDOW *new_win, GX_WINDOW *old_win)
{
    UINT gx_err = GX_SUCCESS;

    if (!new_win->gx_widget_parent)
    {
        gx_err = gx_widget_attach(p_root, (GX_WIDGET *) new_win);
        if (GX_SUCCESS != gx_err) {
            while(1);
        }

    }
    else
    {
        gx_err = gx_widget_show((GX_WIDGET *) new_win);
        if (GX_SUCCESS != gx_err) {
            while(1);
        }

        /** User defined events are routed to the widget that has the current
            focus. Since gx_widget_show does not claim focus we must manually
            claim focus to insure user defined events (e.g temperature updates)
            will be routed to this window properly. */
        gx_err = gx_system_focus_claim(new_win);
        if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
                while(1);
            }

    }
    gx_err = gx_widget_hide((GX_WIDGET *) old_win);
    if (GX_SUCCESS != gx_err) {
            while(1);
        }

}

/*******************************************************************************************************************//**
 * @brief   Displays the appropriate settings screen.
 *
 * @param[in]   new_conent  The id of the old settings window being displayed
 * @param[in]   old_content The id of the new settings window to be displayed
 *
 * @retval      GX_SUCCESS
 ***********************************************************************************************************************/
UINT settings_item_show(GX_WINDOW *widget, USHORT new_content, USHORT old_content) {

    UINT gx_err = GX_SUCCESS;

    GX_WIDGET *pOldContent;
    GX_WIDGET *pNewContent;
    GX_BOOL   return_moved;

    //gx_widget_style_remove(p_child, GX_STYLE_TRANSPARENT);

    /** If we are already on the selected screen, ignore. */
    if(cur_settings_screen == new_content ){
        return gx_err;
    }

    gx_err = gx_widget_find((GX_WIDGET *)widget, old_content, 10, &pOldContent);
    if (GX_SUCCESS != gx_err) {
            while(1);
        }

    gx_err = gx_widget_find((GX_WIDGET *)widget, new_content, 10, &pNewContent);
    if (GX_SUCCESS != gx_err) {
            while(1);
        }

    gx_err = gx_widget_show((GX_WIDGET *)pNewContent);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
            while(1);
        }

    gx_err = gx_widget_hide((GX_WIDGET *)pOldContent);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
            while(1);
        }

    gx_err = gx_widget_front_move(pNewContent, &return_moved);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
            while(1);
        }

    return gx_err;

}
/*******************************************************************************************************************//**
 * @brief   Helper function to update local temperature string.
 *
 * @param[in]   void  No arguments to this function
***********************************************************************************************************************/
static void update_local_temp_string(void) {
    if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
    {
        int integer = (int)floorf(g_gui_state.temp_c);
        int frac    = (int)roundf((g_gui_state.temp_c - (float)integer) * 10.0f);
        snprintf(g_local_temp_str, sizeof(g_local_temp_str), "%d", display_dutycycle);
        snprintf(g_dutycicle_str, sizeof(g_dutycicle_str), "%d", display_dutycycle);
        //snprintf(g_local_temp_str, sizeof(g_local_temp_str), "%2d.%1d", integer, frac);
    }
    else
    {
        int temp_f = (int) c_to_f(g_gui_state.temp_c);
        snprintf(g_local_temp_str, sizeof(g_local_temp_str), "%d", display_dutycycle);
        snprintf(g_dutycicle_str, sizeof(g_dutycicle_str), "%d", display_dutycycle);
        //snprintf(g_local_temp_str, sizeof(g_local_temp_str), "%d", temp_f);
    }


}
/*******************************************************************************************************************//**
 * @brief   Helper function to update target temperature string.
 *
 * @param[in]   void  No arguments to this function
***********************************************************************************************************************/
static void update_target_temp_string(void) {
    /** Update "set to" temperature text. */
    if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
    {
        int integer = (int)floorf(g_gui_state.target_temp);
        int frac    = (int)roundf((g_gui_state.target_temp - (float)integer) * 10.0f);
        snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%2d.%1d", integer, frac);
        snprintf(g_dutycicle_str, sizeof(g_dutycicle_str), "%d", display_dutycycle);
    }
    else
    {
        float temp_f = c_to_f(g_gui_state.target_temp);
        snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%d", (int) temp_f);
        snprintf(g_dutycicle_str, sizeof(g_dutycicle_str), "%d", display_dutycycle);
    }

}

/** @} (end group events) */
