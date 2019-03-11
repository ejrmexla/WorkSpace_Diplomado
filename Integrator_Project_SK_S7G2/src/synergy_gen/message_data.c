/* generated messaging source file - do not edit */
#include "sf_message.h"
#ifndef SF_MESSAGE_CFG_QUEUE_SIZE
#define SF_MESSAGE_CFG_QUEUE_SIZE (16)
#endif
TX_QUEUE system_thread_message_queue;
static uint8_t queue_memory_system_thread_message_queue[SF_MESSAGE_CFG_QUEUE_SIZE];
static sf_message_subscriber_t system_thread_message_queue_0_0 =
{ .p_queue = &system_thread_message_queue, .instance_range =
{ .start = 0, .end = 0 } };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_TOUCH[] =
{ &system_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_TOUCH =
{ .event_class = SF_MESSAGE_EVENT_CLASS_TOUCH, .number_of_nodes = 1, .pp_subscriber_group =
          gp_group_SF_MESSAGE_EVENT_CLASS_TOUCH };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_SYSTEM[] =
{ &system_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_SYSTEM =
{ .event_class = SF_MESSAGE_EVENT_CLASS_SYSTEM, .number_of_nodes = 1, .pp_subscriber_group =
          gp_group_SF_MESSAGE_EVENT_CLASS_SYSTEM };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_VOLUME[] =
{ &system_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_VOLUME =
{ .event_class = SF_MESSAGE_EVENT_CLASS_VOLUME, .number_of_nodes = 1, .pp_subscriber_group =
          gp_group_SF_MESSAGE_EVENT_CLASS_VOLUME };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_TIME[] =
{ &system_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_TIME =
{ .event_class = SF_MESSAGE_EVENT_CLASS_TIME, .number_of_nodes = 1, .pp_subscriber_group =
          gp_group_SF_MESSAGE_EVENT_CLASS_TIME };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_TEMPERATURE[] =
{ &system_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_TEMPERATURE =
{ .event_class = SF_MESSAGE_EVENT_CLASS_TEMPERATURE, .number_of_nodes = 1, .pp_subscriber_group =
          gp_group_SF_MESSAGE_EVENT_CLASS_TEMPERATURE };
static sf_message_subscriber_t *gp_group_SF_MESSAGE_EVENT_CLASS_DISPLAY[] =
{ &system_thread_message_queue_0_0, };
static sf_message_subscriber_list_t g_list_SF_MESSAGE_EVENT_CLASS_DISPLAY =
{ .event_class = SF_MESSAGE_EVENT_CLASS_DISPLAY, .number_of_nodes = 1, .pp_subscriber_group =
          gp_group_SF_MESSAGE_EVENT_CLASS_DISPLAY };
sf_message_subscriber_list_t *p_subscriber_lists[] =
{ &g_list_SF_MESSAGE_EVENT_CLASS_TOUCH, &g_list_SF_MESSAGE_EVENT_CLASS_SYSTEM, &g_list_SF_MESSAGE_EVENT_CLASS_VOLUME,
  &g_list_SF_MESSAGE_EVENT_CLASS_TIME, &g_list_SF_MESSAGE_EVENT_CLASS_TEMPERATURE,
  &g_list_SF_MESSAGE_EVENT_CLASS_DISPLAY, NULL };
void g_message_init(void);
void g_message_init(void)
{
    tx_queue_create (&system_thread_message_queue, (CHAR *) "System Thread Message Queue", 1,
                     &queue_memory_system_thread_message_queue, sizeof(queue_memory_system_thread_message_queue));
}
