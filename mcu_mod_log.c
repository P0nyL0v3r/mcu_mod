/* Includes ------------------------------------------------------------------*/
#include "mcu_mod_conf.h"
#include "mcu_mod_log.h"

#if USE_LOG == 1

#include "usart.h"

#include "string.h"
#include "assert.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdbool.h"

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
static const char * log_level_string[] = {
              "[TRACE]"            " ",
  TERM_BLUE   "[DEBUG]" TERM_RESET " ",
  TERM_GREEN  "[INFO]"  TERM_RESET " ",
  TERM_YELLOW "[WARN]"  TERM_RESET " ",
  TERM_RED    "[ERR]"   TERM_RESET " "
};

#if LOG_USE_STRM_BUF == 1
//for ring buffer
static volatile bool logUseStrmBuf = false;
static TaskHandle_t logTaskHandle;
static StreamBufferHandle_t logStrmBuf;
#endif //#if LOG_USE_STRM_BUF == 1

#if USE_FREERTOS == 1
static SemaphoreHandle_t log_mut;
#endif //#if USE_FREERTOS == 1

#if USE_SPEED_TEST == 1
static uint32_t speed_test_counter = 0;
#endif //#if USE_SPEED_TEST == 1

/* Function prototypes -------------------------------------------------------*/
#if LOG_USE_STRM_BUF == 1
void log_task( void * arg );
#endif//#if LOG_USE_STRM_BUF == 1

/* Functios ------------------------------------------------------------------*/
inline int inIRQ( void )
{
  return __get_IPSR() != 0;
}

void log_lock()
{
  if( !inIRQ() )
    xSemaphoreTake( log_mut, portMAX_DELAY );
}

void log_unlock()
{
  if( !inIRQ() )
    xSemaphoreGive( log_mut );
}

int log_init()
{
  portENTER_CRITICAL( );

  log_mut = xSemaphoreCreateMutex( );

#if LOG_USE_STRM_BUF == 1
  xTaskCreate( log_task, (char*)"[LOG]", LOG_TASK_STACK_SIZE,
  NULL, LOG_TASK_PRIORITY, &logTaskHandle );
#endif

  portEXIT_CRITICAL( );

  return 0;
}

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

int log_puts( const char * str )
{
#if USE_FREERTOS == 1
  log_lock();
#endif//#if USE_FREERTOS == 1

  size_t status = strlen(str);
  _write(0, (char*)str, status);
  _write(0, LOG_EL, sizeof(LOG_EL));

#if USE_FREERTOS == 1
  log_unlock();
#endif//#if USE_FREERTOS == 1

  return status;
}

int log_level( log_level_t level, const char * format, ... )
{
  if( level < LOG_MIN_LEVEL )
    return 0;

#if USE_FREERTOS == 1
  log_lock();
#endif//#if USE_FREERTOS == 1

  fputs( log_level_string[level], stdout );

  int status;
  va_list args;
  va_start( args, format );
  status = vprintf( format, args );
  va_end( args );

  fputs( LOG_EL, stdout );

#if USE_FREERTOS == 1
  log_unlock();
#endif//#if USE_FREERTOS == 1

  return status;
}

int log_write( char * data, int len )
{
  for( int i = 0; i < len; )
  {
#if defined(STM32H7) || defined(STM32G4)
    while( READ_BIT( LOG_UART->ISR, USART_ISR_TXE_TXFNF ) )
      WRITE_REG( LOG_UART->TDR, data[i++] );
#else
    while( READ_BIT( LOG_UART->SR,USART_SR_TXE ) )
      WRITE_REG( LOG_UART->DR,data[i++] );
#endif//#if defined(STM32H7) || defined(STM32G4)
  }

  return len;
}

int _write( int file, char * ptr, int len )
{
  int status = 0;
#if LOG_USE_STRM_BUF == 1
  if(logUseStrmBuf)
  {
    if(inIRQ())
    {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      status = xStreamBufferSendFromISR( logStrmBuf, ptr, len,
                                       &xHigherPriorityTaskWoken );
    }
    else
      status = xStreamBufferSend( logStrmBuf, ptr, len, portMAX_DELAY );
  }
  else
#endif
    status = log_write( ptr, len );

  return status;
}

#if LOG_USE_STRM_BUF == 1

void log_stream_buffer_enable()
{
  logUseStrmBuf = true;
}

void log_stream_buffer_disable()
{
  logUseStrmBuf = false;
}

void log_task( void * arg )
{
  logStrmBuf = xStreamBufferCreate( LOG_STRM_BUF_SIZE, 1 );

  const int bufSize = LOG_TASK_STACK_SIZE * 2;
  char buf[bufSize];
  size_t size;

  log_stream_buffer_enable();

  for( ;; )
  {
    size = xStreamBufferReceive( logStrmBuf, buf, bufSize, portMAX_DELAY );
    log_write( buf, size );
  }
}

#endif//#if LOG_USE_STRM_BUF == 1

#if USE_SPEED_TEST == 1

void speed_test_start()
{
  log_debug( "speed test start" );
  speed_test_counter = dwt_get_counter();
}

void speed_test_stop()
{
  uint32_t clk = dwt_get_counter() - speed_test_counter;
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
  //make attention
  #if LOG_USE_STRM_BUF == 1
    log_stream_buffer_disable();
  #endif
  assert_attention();
  __disable_irq();
  int i = 0, j = 0;
  for( ;; i++ )
  {
    if( i == 0 )
    {
      if( j % 50 == 0 )
      {
        //write in console
        log_err( "ASSERT file: %s", filename );

        log_err( "line: %d", line );

        log_err( "code: %s", expr );

        log_err( "func: %s", assert_func );
      }
    }
    else if( i >= (int)( SystemCoreClock / 100 ) )
    { //delay like
      i = -1;
      j++;
    }
  }
}

#endif//#if USE_LOG == 1
