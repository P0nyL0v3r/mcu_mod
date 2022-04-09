/*
 * mcu_mod_conf.h
 *      Author: Alekseev Aleksei
 * Description:
 */
#ifndef MCU_MOD_CONF_H_
#define MCU_MOD_CONF_H_

//use debug
//what to do:
//	*	define log interface
#define		USE_LOG			0

#if USE_LOG == 1
#define 	LOG_MIN_LEVEL   LOG_DEBUG
#define 	LOG_EL			"\r\n"
//#define 	LOG_UART 		USART1

//use code execution speed test
#define		USE_SPEED_TEST	0
#endif//#if USE_LOG == 1

//us delay
#define		USE_DELAY_US	0

#if USE_DELAY_US == 1
//#define TIM_US	TIM1
#endif


// use rtos
// what to do:
//	*	add to linker arg:
//		-Wl,--wrap=malloc
//		-Wl,--wrap=free
//		-Wl,--wrap=realloc
//
#define		USE_FREERTOS	0

#if USE_FREERTOS == 1
	#include "FreeRTOSConfig.h"

//run time stat
// what to do:
//	* chane variable (in case -oSize optimization):
//		ulTotalRunTime -> volatile ulTotalRunTime
//	* set in FreeRTOSConfig.h:
//		#define configGENERATE_RUN_TIME_STATS            1
//		#define configUSE_TRACE_FACILITY                 1
//		#define configUSE_STATS_FORMATTING_FUNCTIONS     1
//	* add to FreeRTOSConfig.h
//		#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS configureTimerForRunTimeStats
//		#define portGET_RUN_TIME_COUNTER_VALUE getRunTimeCounterValue
//	* HAL
//		* #define RUNTIME_TIMER_HAL htim*
//		* add in timer handler:
//		void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//		{...
//		  if (htim->Instance == RUNTIME_TIMER.Instance) {
//			  ulHighFrequencyTimerTicks++;
//		  }
//		...}
//	* LL
//		* #define RUNTIME_TIMER_LL TIM*
//		* add in timer handler:
//		void TIM*_IRQHandler(void)
//		{
//			if(LL_TIM_IsActiveFlag_UPDATE(RUNTIME_TIMER_LL)) {
//				  LL_TIM_ClearFlag_UPDATE(RUNTIME_TIMER_LL);
//				  ulHighFrequencyTimerTicks++;
//			}
//		}

//#define RUNTIME_TIMER_HAL htim*
//#defime RUNTIME_TIMER_LL  TIM*

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



#endif /*MCU_MOD_CONF_H_*/
