/*
 * Ram.h
 *
 *  Created on: Mar 14, 2019
 *      Author: JGFMEXID
 */
#include "GlobalDefine.h";
#include "stdint.h";

#ifndef RAM_H_
#define RAM_H_
#define C_ONE_SECOND (uint16_t)(1000)
#define C_SECONDS_IN_A_MINUTE (uint8_t)(60)
#define MICRO_SECONDS_IN_A_MINUTE   60000000
#define C_TRANSITIONS_PER_REVOLUTION (uint8_t)(4)
#define TIMER_SECOND (1000000000U)
#define TIMER_MILLISECOND (1000000U)
#define TIMER_MICROSECOND (1000U)
#define BIT_32 (0x100000000U)

#define MAGNET_POLES        4
#define SCALED_FACTOR       1000

extern uint64_t u64time_captured_ns;
extern uint64_t u32TimeCaptured_us;
extern uint16_t u16HallCnt;
extern uint32_t u32Inst_RPM;
extern uint16_t u16EdgeCnt;
extern uint64_t u64capturems;
extern uint32_t u32Filtered_RPM;

extern uint16_t i16InputVar;
extern uint32_t i32VarAccumulator;
extern uint16_t i16AverageVar;

extern uint64_t capture_overflow;
extern uint32_t capture_counter;

/***********************************************************************
             Digital Filter module functions
***********************************************************************/
#define C_FILTER_STATE_INIT     (uint8_t)0
#define C_FILTER_STATE_PROCESS  (uint8_t)1

#define C_FILTER_MIN_SAMPLES    (uint8_t)4
#define C_FILTER_SCALE_FACTOR   (uint16_t)128
#define C_FILTER_RATIO          (uint16_t)256

#define C_LIMIT_VALUE_RANGE_1     (uint32_t) 500    //150 RPM
#define C_LIMIT_VALUE_RANGE_2     (uint32_t) 1000    //300 RPM
#define C_LIMIT_VALUE_RANGE_3     (uint32_t) 1500    //450 RPM
#define C_LIMIT_VALUE_RANGE_4     (uint32_t) 2000    //600 RPM
#define C_LIMIT_VALUE_RANGE_5     (uint32_t) 2500    //750 RPM
#define C_LIMIT_VALUE_RANGE_6     (uint32_t) 3000    //900 RPM

#define C_SPEED_RANGE_1           (uint8_t)1
#define C_SPEED_RANGE_2           (uint8_t)2
#define C_SPEED_RANGE_3           (uint8_t)3
#define C_SPEED_RANGE_4           (uint8_t)4
#define C_SPEED_RANGE_5           (uint8_t)5
#define C_SPEED_RANGE_6           (uint8_t)6

#define C_FILTER_ORDER_FOR_RANGE_1 (uint16_t) 32
#define C_FILTER_ORDER_FOR_RANGE_2 (uint16_t) 32
#define C_FILTER_ORDER_FOR_RANGE_3 (uint16_t) 32
#define C_FILTER_ORDER_FOR_RANGE_4 (uint16_t) 128
#define C_FILTER_ORDER_FOR_RANGE_5 (uint16_t) 128
#define C_FILTER_ORDER_FOR_RANGE_6 (uint16_t) 128

extern void SR_Init_Ram(void);


#endif /* RAM_H_ */
