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
* File Name    : volume.c
* Description  : Brightness settings.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           TODO       1.00    Initial Release.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup  Brightness
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "audio.h"
#include "system_thread.h"
#if AUDIO

/**********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
Public Functions
***********************************************************************************************************************/
void volume_down(uint8_t * p_volume)
{
    int8_t volume = (int8_t)*p_volume;
    volume = (int8_t)(volume - VOLUME_INC);
    if (volume < VOLUME_MIN)
    {
        volume = VOLUME_MIN;
    } else
    if (volume > (int8_t)*p_volume)
    {
        /* Underflow occurred. */
        volume = VOLUME_MIN;
    }

    uint8_t scaled_volume = (uint8_t)(255u - ((255u * (100u - (uint8_t)volume)) / 100u));
    ssp_err_t err = g_sf_audio_playback.p_api->volumeSet(g_sf_audio_playback.p_ctrl, scaled_volume);
    if (SSP_SUCCESS == err)
    {
    	*p_volume = (uint8_t)volume;
    }
}

void volume_up(uint8_t * p_volume)
{
    int8_t volume = (int8_t)*p_volume;
    volume = (int8_t)(volume + VOLUME_INC);
    if ((uint8_t)volume > VOLUME_MAX)
    {
        volume = VOLUME_MAX;
    } else
    if (volume < *p_volume) {
        /* Overflow occurred. */
        volume = VOLUME_MAX;
    }

    uint8_t scaled_volume = (uint8_t)(255u - ((255u * (100u - (uint8_t)volume)) / 100u));
    ssp_err_t err = g_sf_audio_playback.p_api->volumeSet(g_sf_audio_playback.p_ctrl, scaled_volume);
    if (SSP_SUCCESS == err)
    {
    	*p_volume = (uint8_t)volume;
    }
}

void audio_click_init(sf_audio_playback_data_t ** pp_data)
{
    ssp_err_t ssp_err;
	/** Get pointer allocated by application. */
	sf_audio_playback_data_t * p_data = NULL;

#if defined(BSP_BOARD_S5D9_DK)
    /** Hardware enable pin for DAC speaker */
    g_ioport_on_ioport.pinWrite(IOPORT_PORT_00_PIN_14, IOPORT_LEVEL_HIGH);
    ssp_err = g_ioport_on_ioport.pinWrite(IOPORT_PORT_00_PIN_15, IOPORT_LEVEL_HIGH);
    if(SSP_SUCCESS != ssp_err)
    {
        while(1);
    }
#endif

#if !defined(BSP_BOARD_S5D9_DK)
    /** Hardware enable pin for DAC speaker */
	g_ioport_on_ioport.pinWrite(IOPORT_PORT_10_PIN_04, IOPORT_LEVEL_HIGH);
    ssp_err = g_ioport_on_ioport.pinWrite(IOPORT_PORT_09_PIN_02, IOPORT_LEVEL_HIGH);
    if(SSP_SUCCESS != ssp_err)
    {
        while(1);
    }
#endif

    /** Set initial volume. */
	ssp_err = g_sf_audio_playback.p_api->volumeSet(g_sf_audio_playback.p_ctrl, 46);
    if(SSP_SUCCESS != ssp_err)
    {
        while(1);
    }

    /** Get a buffer to make the data. */
    sf_message_acquire_cfg_t acquire_cfg =
    {
    	.buffer_keep = true,
    };
    ssp_err_t err = g_sf_message.p_api->bufferAcquire(g_sf_message.p_ctrl, (sf_message_header_t **) &p_data, &acquire_cfg, TX_NO_WAIT);
    if(SSP_SUCCESS != err)
    {
        while(1);
    }

    p_data->p_data = &click[0];
    p_data->size_bytes = AUDIO_CLICK_BUFFER_LENGTH;
    p_data->type.is_signed = true;
    p_data->type.scale_bits_max = 16;
    p_data->loop_timeout = TX_NO_WAIT;
    p_data->stream_end = true;

    *pp_data = p_data;
}

#endif // AUDIO

/*******************************************************************************************************************//**
 * @}
***********************************************************************************************************************/
