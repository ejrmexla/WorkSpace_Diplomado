/* generated messaging header file - do not edit */
#ifndef SF_MESSAGE_PORT_H_
#define SF_MESSAGE_PORT_H_
typedef enum e_sf_message_event_class
{
    SF_MESSAGE_EVENT_CLASS_TOUCH, /* Touch */
    SF_MESSAGE_EVENT_CLASS_SYSTEM, /* System */
    SF_MESSAGE_EVENT_CLASS_VOLUME, /* Volume */
    SF_MESSAGE_EVENT_CLASS_TIME, /* Time */
    SF_MESSAGE_EVENT_CLASS_TEMPERATURE, /* Temperature */
    SF_MESSAGE_EVENT_CLASS_DISPLAY, /* Display */
} sf_message_event_class_t;
typedef enum e_sf_message_event
{
    SF_MESSAGE_EVENT_UNUSED, /* Unused */
    SF_MESSAGE_EVENT_NEW_DATA, /* New Data */
    SF_MESSAGE_EVENT_REFRESH, /* Refresh */
    SF_MESSAGE_EVENT_TEMPERATURE_INCREMENT, /* Temperature Increment */
    SF_MESSAGE_EVENT_TEMPERATURE_DECREMENT, /* Temperature Decrement */
    SF_MESSAGE_EVENT_HOUR_MODE_TOGGLE, /* Hour Mode Toggle */
    SF_MESSAGE_EVENT_AM_PM_TOGGLE, /* AM PM Toggle */
    SF_MESSAGE_EVENT_SET_TIME, /* Set Time */
    SF_MESSAGE_EVENT_SET_DATE, /* Set Date */
    SF_MESSAGE_EVENT_FAN_TOGGLE, /* Fan Toggle */
    SF_MESSAGE_EVENT_SYSTEM_MODE_TOGGLE, /* System Mode Toggle */
    SF_MESSAGE_EVENT_TEMPERATURE_UNIT_C, /* Temperature Unit C */
    SF_MESSAGE_EVENT_TEMPERATURE_UNIT_F, /* Temperature Unit F */
    SF_MESSAGE_EVENT_BRIGHTNESS_INCREMENT, /* Brightness Increment */
    SF_MESSAGE_EVENT_BRIGHTNESS_DECREMENT, /* Brightness Decrement */
    SF_MESSAGE_EVENT_VOLUME_INCREMENT, /* Volume Increment */
    SF_MESSAGE_EVENT_VOLUME_DECREMENT, /* Volume Decrement */
    SF_MESSAGE_EVENT_UPDATE_TEMPERATURE, /* Update Temperature */
    SF_MESSAGE_EVENT_UPDATE_TIME, /* Update Time */
    SF_MESSAGE_EVENT_UPDATE_DATE, /* Update Date */
} sf_message_event_t;
extern TX_QUEUE system_thread_message_queue;
#endif /* SF_MESSAGE_PORT_H_ */
