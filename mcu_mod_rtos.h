#ifndef MCU_MOD_MCU_MOD_RTOS_H_
#define MCU_MOD_MCU_MOD_RTOS_H_

#ifdef __cplusplus
extern "C" {
#endif//#ifdef __cplusplus

#include "mcu_mod.h"
#include "mcu_mod_conf.h"

#if USE_FREERTOS == 1

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "message_buffer.h"
#include "stream_buffer.h"
#include "timers.h"

//runtime counter
#if configGENERATE_RUN_TIME_STATS == 1
  extern volatile unsigned long ulHighFrequencyTimerTicks;
#endif//#if configGENERATE_RUN_TIME_STATS == 1

#ifdef __cplusplus

inline void * operator new( size_t sz )
{
  return pvPortMalloc( sz );
}

inline void * operator new[]( size_t sz )
{
  return pvPortMalloc( sz );
}

inline void operator delete( void * p )
{
  vPortFree( p );
}

inline void operator delete[]( void * p )
{
  vPortFree( p );
}

#endif//#ifdef __cplusplus

//heap wrapper
void __wrap_free( void * p );
void* __wrap_malloc( size_t xSize );
void* __wrap_realloc( void * ptr, size_t size );

typedef enum
{
  rtosPriorityIdle = tskIDLE_PRIORITY,            //!< priority: idle (lowest)
  rtosPriorityLow,        			  //!< priority: low
  rtosPriorityBelowNormal,         		  //!< priority: below normal
  rtosPriorityNormal,          			  //!< priority: normal (default)
  rtosPriorityAboveNormal,          		  //!< priority: above normal
  rtosPriorityHigh,          		          //!< priority: high
  rtosPriorityRealtime = configMAX_PRIORITIES - 1,//!< priority: realtime (highest)
} rtosPriority_t;

#endif//#if USE_FREERTOS == 1

#ifdef __cplusplus
}
#endif//#ifdef __cplusplus

#endif /* MCU_MOD_MCU_MOD_RTOS_H_ */
