/* generated messaging header file - do not edit */
#ifndef SF_MESSAGE_PAYLOADS_H_
#define SF_MESSAGE_PAYLOADS_H_
#include "sf_touch_panel_api.h"
#include "system_api.h"
#include "volume_api.h"
#include "time_api.h"
#include "temperature_api.h"
#include "display_api.h"
typedef union u_sf_message_payload
{
    sf_touch_panel_payload_t sf_touch_panel_payload;
    system_payload_t system_payload;
    volume_payload_t volume_payload;
    time_payload_t time_payload;
    temperature_payload_t temperature_payload;
    display_payload_t display_payload;
} sf_message_payload_t;
#endif /* SF_MESSAGE_PAYLOADS_H_ */
