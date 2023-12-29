#include "mcu_mod_rtos.h"

#include "assert.h"
#include "string.h"
#include "stdlib.h"

#if USE_FREERTOS == 1

#if configGENERATE_RUN_TIME_STATS == 1

#include "tim.h"

volatile unsigned long ulHighFrequencyTimerTicks = 0;

void configureTimerForRunTimeStats( void )
{
#if defined RUNTIME_TIMER_HAL
  HAL_TIM_Base_Start_IT( &RUNTIME_TIMER_HAL );
#elif defined RUNTIME_TIMER_LL
  LL_TIM_EnableIT_UPDATE( RUNTIME_TIMER_LL );
  LL_TIM_EnableCounter( RUNTIME_TIMER_LL );
#endif
}

unsigned long getRunTimeCounterValue( void )
{
  return ulHighFrequencyTimerTicks;
}

#endif

void* __wrap_malloc( size_t xSize )
{
  return pvPortMalloc( xSize );
}

void __wrap_free( void * p )
{
  vPortFree( p );
}

void* __wrap_realloc( void * ptr, size_t size )
{
  void * new_ptr = pvPortMalloc( size );
  memcpy( new_ptr, ptr, size );
  vPortFree( ptr );
  return new_ptr;
}

#endif
