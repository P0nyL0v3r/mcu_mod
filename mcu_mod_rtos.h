/*
 *      Author: Alekseev A.R.
 */

#ifndef MCU_MOD_MCU_MOD_RTOS_H_
#define MCU_MOD_MCU_MOD_RTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_mod.h"

#if USE_FREERTOS == 1

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//generate run time stats
#if configGENERATE_RUN_TIME_STATS != 1 && configUSE_TRACE_FACILITY !=1 && configUSE_STATS_FORMATTING_FUNCTIONS != 1
	#pragma message "configGENERATE_RUN_TIME_STATS"
#endif

//record stack high address
#if configRECORD_STACK_HIGH_ADDRESS == 0
	#pragma message "configRECORD_STACK_HIGH_ADDRESS"
#endif

//check stack overflow
#if configCHECK_FOR_STACK_OVERFLOW == 0
	#pragma message "configCHECK_FOR_STACK_OVERFLOW"
#endif

//malloc failed hook
#if configUSE_MALLOC_FAILED_HOOK == 0
	#pragma message "configUSE_MALLOC_FAILED_HOOK"
#endif

//runtime counter
#if configGENERATE_RUN_TIME_STATS == 1
	extern volatile unsigned long ulHighFrequencyTimerTicks;
#endif

//heap wrapper
	void __wrap_free( void* p);
	void * __wrap_malloc( size_t xSize );
	void * __wrap_realloc(void * ptr, size_t size);

	typedef enum {
	  rtosPriorityIdle         = tskIDLE_PRIORITY,          ///< priority: idle (lowest)
	  rtosPriorityLow          ,        					///< priority: low
	  rtosPriorityBelowNormal  ,         					///< priority: below normal
	  rtosPriorityNormal       ,          					///< priority: normal (default)
	  rtosPriorityAboveNormal  ,          					///< priority: above normal
  rtosPriorityHigh         ,          						///< priority: high
	  rtosPriorityRealtime     = configMAX_PRIORITIES -1,   ///< priority: realtime (highest)
	}rtosPrio;

#endif

#ifdef __cplusplus
}
#endif

#endif /* MCU_MOD_MCU_MOD_RTOS_H_ */
