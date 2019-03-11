/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the �contents�) are proprietary and confidential to Renesas Electronics Corporation 
 * and/or its licensors (�Renesas�) and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED �AS IS� WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : system_time.c
 * Description  : TODO
 ***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *           TODO       1.00    Initial Release.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @defgroup Time_App_Code
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#define SYS_TIME_C
#include "system_time.h"
#include "sf_message_payloads.h"
#include "system_thread.h"

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
extern const sf_message_post_cfg_t g_post_cfg;
extern const sf_message_acquire_cfg_t g_acquire_cfg;

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define UNUSED(x) (void)(x)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static const uint8_t month_table[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static const uint8_t ly_month_table[12] = {6, 2, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static const uint8_t days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/***********************************************************************************************************************
 * Private functions
 **********************************************************************************************************************/
static int weekday_get(rtc_time_t * p_time);
static void time_validate(int * time, int max);

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
volatile bool g_time_updated = false;

/*******************************************************************************************************************//**
 * @brief Callback used by the RTC to update the system time.
 *
 * @param[in] p_context User-supplied context carrying any data needed during callback processing.
 **********************************************************************************************************************/
void time_update_callback (rtc_callback_args_t * p_context)
{
    UNUSED(p_context);      // Eliminate warning from unused arg
	time_message_post();
}

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Corrects the time value if it has wrapped above the maximum value or below 0.
 *
 * @param[in]   p_time  Time value
 * @param[in]   max     Maximum value for this time
***********************************************************************************************************************/
static void time_validate(int * p_time, int max)
{
    if (*p_time < 0)
    {
        *p_time = max - 1;
    }
    if (*p_time >= max)
    {
        *p_time = 0;
    }
}

/*******************************************************************************************************************//**
 * @brief   Modifies the current time variable by the amount of time requested.  Wrap around events will not affect the
 *
 *
 * @param[in]   years   number of years to adjust by (signed value)
 * @param[in]   months  number of months to adjust by (signed value)
 * @param[in]   days    number of days to adjust by (signed value)
 * @param[in]   hours   number of hours to adjust by (signed value)
 * @param[in]   mins    number of minutes to adjust by (signed value)
 * @param[in]   secs    number of seconds to adjust by (signed value)
***********************************************************************************************************************/
void adjust_time(rtc_time_t * p_time)
{
    rtc_time_t t;
    g_rtc.p_api->calendarTimeGet(g_rtc.p_ctrl, &t);
    t.tm_sec    += p_time->tm_sec;
    t.tm_min    += p_time->tm_min;
    t.tm_hour   += p_time->tm_hour;
    t.tm_mday   += p_time->tm_mday;
    t.tm_mon    += p_time->tm_mon;
    t.tm_year   += p_time->tm_year;

    bool leap = false;
    if (0 == t.tm_year % 4)
    {
        leap = true;
        if (0 == t.tm_year % 100)
        {
            leap = false;
            if (0 != t.tm_year % 400)
            {
                leap = true;
            }
        }
    }

    /** Ensure that all time values are valid. */
    time_validate(&t.tm_sec, 60);
    time_validate(&t.tm_min, 60);
    time_validate(&t.tm_hour, 24);
    time_validate(&t.tm_mon, 12);

    /** Calculate the maximum days in the current month. */
    int32_t temp_month = (t.tm_mon + 12) % 12;
    uint8_t days_in_current_month = days_in_month[temp_month];
    if (leap && (1 == temp_month))
    {
        days_in_current_month++;
    }
    //This fixes for 1.2.0
    time_validate(&t.tm_mday, days_in_current_month+1);
    t.tm_wday = weekday_get(&t);

    g_rtc.p_api->calendarTimeSet(g_rtc.p_ctrl, &t, true);

    /** Post message to system. */
    time_message_post();
}

int weekday_get(rtc_time_t * p_time)
{
    int32_t day = p_time->tm_mday;
    bool leap = false;
    if (0 == p_time->tm_year % 4)
    {
        leap = true;
        if (0 == p_time->tm_year % 100)
        {
            leap = false;
            if (0 != p_time->tm_year % 400)
            {
                leap = true;
            }
        }
    }
    int32_t month_offset;
    if (leap)
    {
        month_offset = ly_month_table[p_time->tm_mon];
    }
    else
    {
        month_offset = month_table[p_time->tm_mon];
    }
    int32_t year_offset = p_time->tm_year % 100;
    int32_t century_offset = (3 - ((p_time->tm_year / 100) % 4)) * 2;
    int32_t offset = day + month_offset + year_offset + year_offset / 4 + century_offset;
    int32_t index = offset % 7;

    return index;
}

/*******************************************************************************************************************//**
 * @brief   Posts time update message to messaging framework.
***********************************************************************************************************************/
void time_message_post(void)
{
    sf_message_header_t * p_message  = NULL;
    sf_message_payload_t * p_payload = NULL;
	/** Get buffer from messaging framework. */
	ssp_err_t err;
	sf_message_post_err_t post_err;
	err = g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl, (sf_message_header_t **) &p_message, &g_acquire_cfg, TX_NO_WAIT);
	if (SSP_SUCCESS != err)
	{
		/** TODO_THERMO: Send error message. */
	    while(1);
	}

	/** Create message in buffer. */
	p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
	p_message->event_b.code  = SF_MESSAGE_EVENT_UPDATE_TIME;
    p_payload = (sf_message_payload_t*)(p_message+1);
	g_rtc.p_api->calendarTimeGet(g_rtc.p_ctrl, &p_payload->time_payload.time);

	/** Post message. */
	err = g_sf_message.p_api->post(g_sf_message.p_ctrl, (sf_message_header_t *) p_message, &g_post_cfg, &post_err, TX_NO_WAIT);
	if (SSP_SUCCESS != err)
	{
	    while(1);
	}
}
#if 0
void system_time_init(void)
{
}
#elif 1
void system_time_init(void)
{
    ssp_err_t error;
    rtc_time_t t = {0};

    error = g_rtc.p_api->open(g_rtc.p_ctrl, g_rtc.p_cfg);
    if (SSP_SUCCESS == error)
    {
        g_rtc.p_api->calendarCounterStart(g_rtc.p_ctrl);

        rtc_time_t current_time;
        g_rtc.p_api->calendarTimeGet(g_rtc.p_ctrl, &current_time);

        adjust_time(&t);

        t.tm_wday = weekday_get(&current_time);

        g_rtc.p_api->periodicIrqRateSet(g_rtc.p_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
        error = g_rtc.p_api->irqEnable(g_rtc.p_ctrl, RTC_EVENT_PERIODIC_IRQ);
        if (error != SSP_SUCCESS)
        {
            while(1);
            /** TODO_THERMO: Post error message. */
        }
    } else {
        while(1);
    }
}
#endif
/** @} (end) */
