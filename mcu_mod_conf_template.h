#ifndef MCU_MOD_CONF_H_
#define MCU_MOD_CONF_H_

//bcd version
#define 	MCU_MOD			0x10

//use debug
//what to do:
//	*	define log interface
#define		USE_LOG			0

#if USE_LOG == 1
//minimal log level for
#define 	LOG_MIN_LEVEL      	LOG_TRACE
//endline code in end of string
#define 	LOG_EL			"\r\n"
//use ring buffer
#define		LOG_USE_STRM_BUF 	0

#if LOG_USE_STRM_BUF == 1
//ring buffer size
#define 	LOG_STRM_BUF_SIZE	256
//task priority to process ring buffer
#define 	LOG_TASK_PRIORITY	rtosPriorityLow
//task stack size to process ring buffer
#define 	LOG_TASK_STACK_SIZE 	configMINIMAL_STACK_SIZE
#endif

//UART interface for log output
#define 	LOG_UART		USARTx
//use code execution speed test
#define		USE_SPEED_TEST	  	0
#endif//#if USE_LOG == 1

//us delay
#define		USE_DELAY_US	   	0
//timer for us delay. if not defined => DWT will be used
//#define 	TIM_US	          	TIM1

// use rtos
// what to do:
// * add to linker arg:
//    -Wl,--wrap=malloc
//    -Wl,--wrap=free
//    -Wl,--wrap=realloc
#define		USE_FREERTOS	    	0

#if USE_FREERTOS == 1

#include "FreeRTOSConfig.h"

//run time stat
// what to do:
//* chane variable (in case -oSize optimization):
//  ulTotalRunTime -> volatile ulTotalRunTime
//* set in FreeRTOSConfig.h:
//  #define configGENERATE_RUN_TIME_STATS            1
//  #define configUSE_TRACE_FACILITY                 1
//  #define configUSE_STATS_FORMATTING_FUNCTIONS     1
//* add to FreeRTOSConfig.h
//  #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS configureTimerForRunTimeStats
//  #define portGET_RUN_TIME_COUNTER_VALUE getRunTimeCounterValue
//* HAL
//  * #define RUNTIME_TIMER_HAL htim*
//  * add in timer handler:
//  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//  {...
//    if (htim->Instance == RUNTIME_TIMER_HAL.Instance) {
//      ulHighFrequencyTimerTicks++;
//    }
//  ...}
//* LL
//  * #define RUNTIME_TIMER_LL TIM*
//  * add in timer handler:
//  void TIM*_IRQHandler(void)
//  {
//    if(LL_TIM_IsActiveFlag_UPDATE(RUNTIME_TIMER_LL)) {
//      LL_TIM_ClearFlag_UPDATE(RUNTIME_TIMER_LL);
//      ulHighFrequencyTimerTicks++;
//    }
//  }

#define 	RUNTIME_TIMER_HAL	htimx
//#defime 	RUNTIME_TIMER_LL  	TIM*

//check stack high address
//what to do:
//	* set in FreeRTOSConfig.h:
//		#define configRECORD_STACK_HIGH_ADDRESS          1

//check stack overflow
//what to do:
//	* set in FreeRTOSConfig.h:
//		#define configCHECK_FOR_STACK_OVERFLOW          1
//	*	define void vApplicationStackOverflowHook(void) {}

//malloc failed hook
//what to do:
//	* set in FreeRTOSConfig.h:
//		#define configUSE_MALLOC_FAILED_HOOK            1
//	*	define void vApplicationMallocFailedHook(void) {}

#endif//#if USE_FREERTOS == 1

//check settings
#if USE_LOG == 1 && USE_SPEED_TEST == 1 && !defined(USE_DWT)
#define 	USE_DWT 		1
#endif

#if USE_DELAY_US == 1 && !defined(TIM_US) && !defined(USE_DWT)
#define 	USE_DWT 		1
#endif

#endif /*MCU_MOD_CONF_H_*/
