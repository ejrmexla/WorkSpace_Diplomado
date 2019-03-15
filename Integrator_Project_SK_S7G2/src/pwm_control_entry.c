/*DC Motor Control entry function*/
#include "pwm_control.h"
#include "GlobalDefine.h"
#include <stdio.h>


uint8_t dutycycle = 100;
  void pwm_control_entry(void)
  {
       //open timer object
       g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);

       //Change Duty cycle around 30% i.e. overriding properties
       g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl, (30 %100), TIMER_PWM_UNIT_PERCENT, CLOCK_B);

       //start the timer which will product PWM
       g_timer0.p_api->start(g_timer0.p_ctrl);
       //tx_thread_sleep (150);
       //dutycycle = (dutycycle  + 5) % 100; // Every iteration change Duty Cycle 5% more
       g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl, dutycycle, TIMER_PWM_UNIT_PERCENT, CLOCK_B);
       display_dutycycle = dutycycle;
       snprintf(g_dutycicle_str, sizeof(g_dutycicle_str), "%d", display_dutycycle);

}
