/* generated thread header file - do not edit */
#ifndef PWM_CONTROL_H_
#define PWM_CONTROL_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
extern "C" void pwm_control_entry(void);
#else
extern void pwm_control_entry(void);
#endif
#include "r_gpt.h"
#include "r_timer_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer0;
#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* PWM_CONTROL_H_ */
