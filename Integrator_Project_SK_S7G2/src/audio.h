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
* File Name    : audio.h
* Description  : Audio application control.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           TODO       1.00    Initial Release.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup Thermostat
 * @defgroup Audio
 *
 * Audio volume settings are controlled in this module.
 *
 * @{
 **********************************************************************************************************************/

#ifndef AUDIO_H
#define AUDIO_H

#include "system_cfg.h"
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#if AUDIO

#include "system_api.h"


/** Included for access to audio control block. */
#include "system_thread.h"

/**********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define AUDIO_CLICK_BUFFER_LENGTH (1416)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
/** Raw click PCM data sampled at 22050 kHz. */
extern unsigned char click[];

/***********************************************************************************************************************
Function prototypes
***********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief  Increases audio volume within allowable configurable range and updates state.
 *
 * @param[in,out]  p_volume  Current volume.
***********************************************************************************************************************/
void volume_up(uint8_t * p_volume);

/*******************************************************************************************************************//**
 * @brief  Decreases audio volume within allowable configurable range and updates state.
 *
 * @param[in,out]  p_volume  Current volume.
***********************************************************************************************************************/
void volume_down(uint8_t * p_volume);

/*******************************************************************************************************************//**
 * @brief  Initializes click data message.
 *
 * @param[in,out]  p_volume  Current volume.
***********************************************************************************************************************/
void audio_click_init(sf_audio_playback_data_t ** p_data);

#endif // AUDIO

#endif /* AUDIO_H */

/*******************************************************************************************************************//**
 * @}
***********************************************************************************************************************/
