/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : system_cfg.h
 * Description  : Header file for system configurations.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *           06.03.2015 1.00    Initial Release.
 **********************************************************************************************************************/

#ifndef SYSTEM_CFG_H
#define SYSTEM_CFG_H

//#include "bsp_cfg.h"
#include "system_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define TEMP_MAX_C                      (30.0f)
#define TEMP_MIN_C                      (15.0f)
#define TEMP_INC_C                      (0.6f)

#if defined(BSP_BOARD_S7G2_SK)
#define LCD_CONTROLLER_9341V            (1)
#endif

#if !defined(BSP_BOARD_S7G2_SK)
#define PWM_BRIGHTNESS_CONTROL          (1) // 0
#define BRIGHTNESS_DIMMING              (1)
#define BRIGHTNESS_MAX                  (100)
#define BRIGHTNESS_MIN                  (10)
#define BRIGHTNESS_INC                  (5)
#endif

#if defined(BSP_BOARD_S7G2_PE_HMI1)
#define LCD_ON_PIN                      IOPORT_PORT_10_PIN_03
#define LCD_PWM_PIN                     0
#elif defined(BSP_BOARD_S7G2_DK)
#define LCD_ON_PIN                      IOPORT_PORT_07_PIN_10
#define LCD_PWM_PIN                     1
#elif defined(BSP_BOARD_S7G2_SK)
#define LCD_ON_PIN                      IOPORT_PORT_07_PIN_10
#elif defined(BSP_BOARD_S5D9_DK)
#define LCD_ON_PIN                      IOPORT_PORT_07_PIN_08
#define LCD_PWM_PIN                     1
#endif


#define VOLUME_MAX                      (100)
#define VOLUME_MIN                      (0)
#define VOLUME_INC                      (5)

#if !defined(BSP_BOARD_S7G2_SK)
#define AUDIO                           (1)
#define AUDIO_VOLUME_CONTROL_USED       (1)
#define AUDIO_IN_EACH_EVENT             (0)
#else
#define AUDIO                           (0)
#define AUDIO_VOLUME_CONTROL_USED       (0)
#define AUDIO_IN_EACH_EVENT             (0)
#endif

#define SPLASH_TIMEOUT                  (200)

#include "sf_message_payloads.h"

#endif /* SYSTEM_CFG_H */
