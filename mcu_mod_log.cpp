#include <mcu_mod_log.h>
#include "main.h"

#if	defined STM32F4
	#include "core_cm4.h"
	#include "stm32f4xx.h"
#elif defined STM32F7
	#include "core_cm7.h"
	#include "stm32f7xx.h"
#elif defined STM32G0
	#include "core_cm0plus.h"
	#include "stm32g0xx.h"
#elif defined STM32H7
  #include "core_cm7.h"
  #include "stm32h7xx.h"
#elif defined STM32G4
	#include "core_cm4.h"
	#include "stm32g4xx.h"
#else
	#error "undefined core"
#endif

#if USE_LOG == 1

#include "string.h"
#include "assert.h"
#include "stdio.h"
#include "stdarg.h"

static const char * log_level_string[] = {
  "[TRACE]" " ",
  TERM_BLUE "[DEBUG]" TERM_RESET " ",
  TERM_GREEN "[INFO]" TERM_RESET " ",
  TERM_YELLOW "[WARN]" TERM_RESET " ",
  TERM_RED "[ERR]" TERM_RESET " "
};

inline int inIRQ( void )
{
  return __get_IPSR() != 0;
}

#if USE_FREERTOS == 1

void log_lock();
void log_unlock();
int log_init();

//for semaphore init
bool logInitState = false;
static SemaphoreHandle_t log_mut;

#if LOG_USE_STRM_BUF == 1

void log_task( void * arg );

//for ring buffer
bool logUseStrmBuf = false;
TaskHandle_t logTaskHandle;
StreamBufferHandle_t logStrmBuf;

#endif//#if LOG_USE_STRM_BUF == 1

void log_lock()
{
  if( !inIRQ() )
  {
    if( !logInitState )
      log_init();
    xSemaphoreTake( log_mut, portMAX_DELAY );
  }
}

void log_unlock()
{
  if( !inIRQ() )
  {
    xSemaphoreGive( log_mut );
  }
}

int log_init()
{
  portENTER_CRITICAL( );

  if( !logInitState )
  {
    log_mut = xSemaphoreCreateMutex( );

#if LOG_USE_STRM_BUF == 1
    xTaskCreate( log_task, (char*)"[LOG]", LOG_TASK_STACK_SIZE,
    NULL,
                 LOG_TASK_PRIORITY, &logTaskHandle );
#endif

    logInitState = true;
  }

  portEXIT_CRITICAL( );

  return 0;
}

#if LOG_USE_STRM_BUF == 1

void log_task( void * arg )
{
  logStrmBuf = xStreamBufferCreate( LOG_STRM_BUF_SIZE, 1 );
  logUseStrmBuf = true;
  const int bufSize = LOG_TASK_STACK_SIZE * 2;
  char buf[bufSize];
  size_t size;

  for( ;; )
  {
    size = xStreamBufferReceive( logStrmBuf, buf, bufSize, portMAX_DELAY );
    log_write( buf, size );
  }
}

#endif//#if LOG_USE_STRM_BUF == 1

#endif//#if USE_FREERTOS == 1

int log_printf( const char * format, ... )
{
#if USE_FREERTOS == 1
  log_lock();
#endif//#if USE_FREERTOS == 1

  int stat;
  va_list args;
  va_start( args, format );
  stat = vprintf( format, args );
  va_end( args );

#if USE_FREERTOS == 1
  log_unlock();
#endif//#if USE_FREERTOS == 1

  return stat;
}

int log_level( log_level_t level, const char * format, ... )
{
  if( level < LOG_MIN_LEVEL )
    return 0;

#if USE_FREERTOS == 1
  log_lock();
#endif//#if USE_FREERTOS == 1

  fputs( log_level_string[level], stdout );

  int stat;
  va_list args;
  va_start( args, format );
  stat = vprintf( format, args );
  va_end( args );

  fputs( LOG_EL, stdout );

#if USE_FREERTOS == 1
  log_unlock();
#endif//#if USE_FREERTOS == 1

  return stat;
}

int log_write( char * data, int len )
{
#if LOG_USE_ITM == 1 && defined(ITM)

	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(data[DataIdx]);
	}

#endif//#if LOG_USE_ITM == 1 && defined(ITM)

#if defined(LOG_UART)
  for( int i = 0; i < len; )
  {
#if defined(STM32H7) || defined(STM32G4)
    while( READ_BIT( LOG_UART->ISR, USART_ISR_TXE_TXFNF ) )
    {
      WRITE_REG( LOG_UART->TDR, data[i++] );
    }
#else
		while( READ_BIT( LOG_UART->SR,USART_SR_TXE ) )
		{
			WRITE_REG( LOG_UART->DR,data[i++] );
		}
#endif//#if defined(STM32H7) || defined(STM32G4)
  }
#endif//#if defined(LOG_UART)

  return len;
}

int _write( int file, char * ptr, int len )
{
  int stat = 0;

#if LOG_USE_STRM_BUF == 1
  if( logUseStrmBuf && !inIRQ() )
    stat = xStreamBufferSend( logStrmBuf, ptr, len, portMAX_DELAY );
  else
#endif
    stat = log_write( ptr, len );

  return stat;
}

#if USE_SPEED_TEST == 1

void speed_test_start()
{
  log_debug( "speed test start" );
  //https://stackoverflow.com/questions/36378280/stm32-how-to-enable-dwt-cycle-counter
  CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk; //enable trace
#if __CORTEX_M	== (7U)
  __DSB();
  DWT->LAR = 0xC5ACCE55;
  __DSB(); //added unlock access to DWT (ITM, etc.)registers
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //disable DWT cycle counter
#endif
  DWT->CYCCNT = 0; //clear DWT cycle counter
  DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk; //enable DWT cycle counter
}

void speed_test_stop()
{
  uint32_t clk = DWT->CYCCNT;
  uint32_t us = clk / ( SystemCoreClock / 1000000 );
  uint32_t ms = us / 1000;
  uint32_t val;

  char * suf = NULL;

  if( clk < SystemCoreClock / 100000 )
  {
    suf = (char*)"clk";
    val = clk;
  }
  else if( us < 10000 )
  {
    suf = (char*)"us";
    val = us;
  }
  else
  {
    suf = (char*)"ms";
    val = ms;
  }
  log_debug( "speed:%lu %s", val, suf );
}

#endif//#if USE_SPEED_TEST == 1

__attribute__((weak)) void assert_attention()
{
  log_err( "attention" );
}

void __assert_func( const char * filename, int line, const char * assert_func, const char * expr )
{
  //привлекаем внимание
  assert_attention();
  __disable_irq(); //отключаем прерывания, что бы ничего не вызывалось
  int i = 0, j = 0;
  for( ;; i++ )
  {
    if( i == 0 )
    {
      if( j % 50 == 0 )
      {
        //пишем в консольку
        log_err( "ASSERT file: %s", filename );

        log_err( "line: %d", line );

        log_err( "code: %s", expr );

        log_err( "func: %s", assert_func );
      }
    }
    else if( i >= (int)( SystemCoreClock / 100 ) )
    { //типо задержка, только без прерываний
      i = -1;
      j++;
    }
  }
}

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

void hard_fault_handler()
{
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
  assert( "hard fault" );
}

#endif//#if USE_LOG == 1

#if USE_DELAY_US == 1

void delay_us( uint16_t delay ) {
#if !defined( TIM_US ) && defined( DWT )
		uint32_t tickstart = DWT->CYCCNT;
		static uint32_t us_tick = SystemCoreClock / 1000000;
		uint32_t wait = delay * us_tick;
	/* Add a freq to guarantee minimum wait */
		if (wait < 0xFFFFFFFFU)	{
			wait += (uint32_t)( us_tick );
		}
		while ( ( DWT->CYCCNT - tickstart ) < wait ){}
#elif defined(TIM_US)
	//Подразумевается, что таймер инкрементируется каждую микросекунду
		uint32_t tickstart = TIM_US->CNT;
		uint32_t wait = delay;
	/* Add a freq to guarantee minimum wait */
		if (wait < 0xFFFFU)	{
			wait += (uint32_t)1;
		}
		while ((TIM_US->CNT - tickstart) < wait){}
#else
  #error "must to define us timer"
#endif
}

#endif

