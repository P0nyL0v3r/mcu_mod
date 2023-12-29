/* Includes ------------------------------------------------------------------*/
#include "mcu_mod_conf.h"
#include "mcu_mod_debug.h"

#include "main.h"

#if USE_LOG == 1

#if defined STM32F1
    #include "core_cm3.h"
    #include "stm32f1xx.h"
#elif defined STM32F3
  #include "core_cm4.h"
  #include "stm32f3xx.h"
#elif defined STM32F4
  #include "core_cm4.h"
  #include "stm32f4xx.h"
#elif defined STM32F7
  #include "core_cm7.h"
  #include "stm32f7xx.h"
#elif defined STM32H7
  #include "core_cm7.h"
  #include "stm32h7xx.h"
#elif defined STM32G0
  #include "core_cm0plus.h"
  #include "stm32g0xx.h"
#elif defined STM32G4
  #include "core_cm4.h"
  #include "stm32g4xx.h"
#else
  #error "undefined core"
#endif

/* Types ---------------------------------------------------------------------*/

/* Macro ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
#if LOG_USE_STRM_BUF == 1
void log_stream_buffer_disable();
#endif//#if LOG_USE_STRM_BUF == 1

/* Functios ------------------------------------------------------------------*/

void usage_fault_handler( uint32_t CFSRValue )
{
  char * type;
  //CFSRValue >>= 16;                  // right shift to lsb
  if( ( CFSRValue & ( 1 << SCB_CFSR_DIVBYZERO_Pos ) ) != 0 )
  {
    type = (char*)"Divide by zero";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_UNALIGNED_Pos ) ) != 0 )
  {
    type = (char*)"Unaligned access";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_NOCP_Pos ) ) != 0 )
  {
    type = (char*)"No coprocessor";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_INVPC_Pos ) ) != 0 )
  {
    type = (char*)"Invalid PC load";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_INVSTATE_Pos ) ) != 0 )
  {
    type = (char*)"Invalid state";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_UNDEFINSTR_Pos ) ) != 0 )
  {
    type = (char*)"Undefined instruction";
  }
  else
  {
    type = (char*)"undefined";
  }
  log_err( "Usage fault: %s", type );
}

void bus_fault_handler( uint32_t CFSRValue )
{
  char * type;
  //CFSRValue >>= 8;                  // right shift to lsb
  if( ( CFSRValue & ( 1 << SCB_CFSR_IMPRECISERR_Pos ) ) != 0 )
  {
    type = (char*)"Imprecise data bus error";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_PRECISERR_Pos ) ) != 0 )
  {
    type = (char*)"Precise data bus error";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_IBUSERR_Pos ) ) != 0 )
  {
    type = (char*)"Instruction bus error";
  }
  else
  {
    type = (char*)"undefined";
  }
  log_err( "Bus fault: %s", type );
}

void mem_fault_handler( uint32_t CFSRValue )
{
  char * type;
  //CFSRValue >>= 0; // right shift to lsb
  if( ( CFSRValue & ( 1 << SCB_CFSR_DACCVIOL_Pos ) ) != 0 )
  {
    type = (char*)"Data access violation";
  }
  else if( ( CFSRValue & ( 1 << SCB_CFSR_IACCVIOL_Pos ) ) != 0 )
  {
    type = (char*)"Instruction access violation";
  }
  else
  {
    type = (char*)"undefined";
  }
  log_err( "Memory management fault: %s", type );
}

void print_hard_fault_reason()
{

  #if LOG_USE_STRM_BUF == 1
  log_stream_buffer_disable();
  #endif//#if LOG_USE_STRM_BUF == 1

  //https://blog.feabhas.com/2013/02/developing-a-generic-hard-fault-handler-for-arm-cortex-m3cortex-m4/
  log_err( "In Hard Fault Handler" );
  log_err( "SCB->HFSR = 0x%08lX", (uint32_t)SCB->HFSR );
  if( SCB->HFSR & 0x80000000 )
  {
    log_err( "debug event" );
  }
  if( ( SCB->CFSR & 0xFFFF0000 ) != 0 )
  {
    usage_fault_handler( SCB->CFSR );
  }
  if( ( SCB->CFSR & 0x0000FF00 ) != 0 )
  {
    bus_fault_handler( SCB->CFSR );
  }
  if( ( SCB->CFSR & 0x000000FF ) != 0 )
  {
    mem_fault_handler( SCB->CFSR );
  }
  assert( "hard fault" && 0 );
}

uint32_t print_reset_reason()
{
  //https://stackoverflow.com/questions/34196663/stm32-how-to-get-last-reset-status
  char * reason;
  uint32_t flag = 0;

  if( __HAL_RCC_GET_FLAG( RCC_FLAG_LPWRRST ) )
  {
    flag = RCC_FLAG_LPWRRST;
    reason = "reset reason: Low Power reset";
  }
  else if( __HAL_RCC_GET_FLAG( RCC_FLAG_WWDGRST ) )
  {
    flag = RCC_FLAG_WWDGRST;
    reason = "reset reason: Window Watchdog reset";
  }
  else if( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) )
  {
    flag = RCC_FLAG_IWDGRST;
    reason = "reset reason: Independent Watchdog reset";
  }
  else if( __HAL_RCC_GET_FLAG( RCC_FLAG_SFTRST ) )
  {
    flag = RCC_FLAG_SFTRST;
    // This reset is induced by calling the ARM CMSIS
    // `NVIC_SystemReset()` function!
    reason = "reset reason: Software reset";
  }
  else if( __HAL_RCC_GET_FLAG( RCC_FLAG_PORRST ) )
  {
    flag = RCC_FLAG_PORRST;
    reason = "reset reason: POR/PDR reset";
  }
  else if( __HAL_RCC_GET_FLAG( RCC_FLAG_PINRST ) )
  {
    flag = RCC_FLAG_PINRST;
    reason = "reset reason: Pin reset";
  }
  // Needs to come *after* checking the `RCC_FLAG_PORRST` flag in order to
  // ensure first that the reset cause is NOT a POR/PDR reset. See note
  // below.
#if defined RCC_FLAG_BORRST
  else if( __HAL_RCC_GET_FLAG( RCC_FLAG_BORRST ) )
  {
    flag = RCC_FLAG_BORRST;
    reason = "reset reason: POR/PDR or BOR reset";
  }
#endif
  else
  {
    reason = NULL;
  }
  if(reason)
    log_puts(reason);

  // Clear all the reset flags or else they will remain set during future
  // resets until system power is fully removed.
  __HAL_RCC_CLEAR_RESET_FLAGS();

  return flag;
}
#endif//#if USE_LOG == 1

#if USE_DELAY_US == 1

void delay_us( uint16_t delay ) {
#if !defined( TIM_US ) && defined( DWT )
    uint32_t tickstart = dwt_get_counter();
    uint32_t us_tick = SystemCoreClock / 1000000;
    uint32_t wait = delay * us_tick;
  /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFFFFFU) {
      wait += (uint32_t)( us_tick );
    }
    while ( ( dwt_get_counter() - tickstart ) < wait ){}
#elif defined(TIM_US)
  //Подразумевается, что таймер инкрементируется каждую микросекунду
    uint32_t tickstart = TIM_US->CNT;
    uint32_t wait = delay;
  /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFU) {
      wait += (uint32_t)1;
    }
    while ((TIM_US->CNT - tickstart) < wait){}
#else
  #error "must to define us timer"
#endif
}

#endif

#if USE_DWT == 1

inline void dwt_init()
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
#if __CORTEX_M  == (7U)
  DWT->LAR = 0xC5ACCE55;
#endif
  dwt_reset_counter();
  dwt_enable();
}

void dwt_enable()
{
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
void dwt_disable()
{
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
}

inline void dwt_reset_counter()
{
  DWT->CYCCNT = 0;
}

inline uint32_t dwt_get_counter()
{
  return DWT->CYCCNT;
}

#endif
