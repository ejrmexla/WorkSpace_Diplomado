/* Temperature Thread entry function */

#include <stdio.h>
#include <math.h>
#if defined(__ICCARM__)               /* IAR Compiler */
#include <float.h>
#define __FLT_EPSILON__         _FP4_EPSILON     
#endif
#include "sf_message_payloads.h"
#include "temperature_thread.h"

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
extern const sf_message_post_cfg_t g_post_cfg;
extern const sf_message_acquire_cfg_t g_acquire_cfg;

/* Since the framework doesn't generate a function prototype
 * we'll do it here to avoid the warning*/
void temperature_thread_entry(void);

static adc_sample_state_t adc_sample_state = {
    .reg_id     = ADC_SAMPLE_STATE_TEMPERATURE,
    .num_states     = 50       // ch8-20 use the same value
};

#define ADC_SAMPLE_ARRAY_SIZE (16)

static adc_data_size_t adc_data[ADC_SAMPLE_ARRAY_SIZE] = {0};

//static sf_message_payload_temperature_t * p_message = NULL;

void temperature_thread_entry(void)
{

    /** Configure the temperature sensor **/

    /** Start the ADC conversion*/

        /** TODO_THERMO: Post error message and terminate thread. */

    float old_temp = 0;
    bool first = true;
    int32_t ref = 0x5f0;
    int32_t index = 0;
    float temp_in_C = 0;
    ssp_err_t err;

    /** Configure the temperature sensor **/
    err = g_adc.p_api->open(g_adc.p_ctrl, g_adc.p_cfg);
    if (SSP_SUCCESS != err) {
        while(1);
    }
    err = g_adc.p_api->scanCfg(g_adc.p_ctrl, g_adc.p_channel_cfg);
    if (SSP_SUCCESS != err) {
        while(1);
    }

    err = g_adc.p_api->sampleStateCountSet(g_adc.p_ctrl, &adc_sample_state);
    if (SSP_SUCCESS != err) {
        while(1);
    }
    while(1)
    {
        /** Start the ADC conversion*/
       err = g_adc.p_api->scanStart(g_adc.p_ctrl);
        if (SSP_SUCCESS != err) {
            while(1);
        }
        /** Only update every 100 ms. */
        tx_thread_sleep(10);

        /** Read the raw ADC value. */
        g_adc.p_api->read(g_adc.p_ctrl, ADC_REG_TEMPERATURE, &adc_data[index]);
        if (SSP_SUCCESS != err) {
            while(1);
        }

        index++;
        if (index > ADC_SAMPLE_ARRAY_SIZE)
        {
            index = 0;
        }

        /** Read the data from the ADC*/
        int32_t num_samples = 0;
        int32_t total = 0;
        for (uint32_t i = 0; i < ADC_SAMPLE_ARRAY_SIZE; i++)
        {
            if (0 != adc_data[i])
            {
                total += adc_data[i];
                num_samples++;
            }
        }
        int32_t avg = total / num_samples;

        /** Calibrate the temperature sensor to 22 degrees for demo purposes. */
        if (first)
        {
            ref = avg;
            first = false;
        }
        /** Calibrate the temperature sensor to 22 degrees for demo purposes. */

        /** Convert to Celsius using equation: Temp = (measured voltage - reference voltage)/3.8  + Reference temp at reference voltage*/
        temp_in_C = (float) (avg - ref);
        temp_in_C = temp_in_C / 3.8f;
        temp_in_C = temp_in_C + 22;

        if ((int)(temp_in_C * 100) != (int)(old_temp * 100))
        if (fabs(temp_in_C - old_temp) > __FLT_EPSILON__ )

        {
            /** Send event. */
            sf_message_header_t * p_message  = NULL;
            sf_message_payload_t * p_payload = NULL;

            /** Get buffer from messaging framework. */
            err = g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl, (sf_message_header_t **) &p_message, &g_acquire_cfg, TX_NO_WAIT);
            if (SSP_SUCCESS != err)
            {
                while(1);
                /** Post error message. */
            } else {

            /** Create message in buffer. */
            p_message->event_b.class_code = SF_MESSAGE_EVENT_CLASS_TEMPERATURE;
            p_message->event_b.code  = SF_MESSAGE_EVENT_UPDATE_TEMPERATURE;
            p_payload = (sf_message_payload_t*)(p_message+1);
            p_payload->temperature_payload.temperature = temp_in_C;

            /** Post message. */
            sf_message_post_err_t post_err;
            err = g_sf_message.p_api->post(g_sf_message.p_ctrl, (sf_message_header_t *) p_message, &g_post_cfg, &post_err, TX_NO_WAIT);
            if (SSP_SUCCESS != err)
            {
                /** Message is processed, so release buffer. */
                    while(1);
                    /** TODO_THERMO: Post error message. */
                }
                old_temp = temp_in_C;
            }
        }
    }
}
