#ifndef MCU_MOD_MCU_MOD_LOG_H_
#define MCU_MOD_MCU_MOD_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_mod.h"
#include "mcu_mod_conf.h"

//colored tag for debug
#define TERM_RED		  "\033[1;31m"
#define TERM_GREEN		"\033[0;32m"
#define TERM_YELLOW		"\033[0;33m"
#define TERM_BLUE		  "\033[0;34m"
#define TERM_RESET		"\033[0;0m"

typedef enum
{
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERR
} log_level_t;

//! return true, if it called in IRQ
int inIRQ( void );

#if USE_LOG == 1

//! message output without endline
int log_printf( const char * format, ... );

//! message output with level and endline
int log_level( log_level_t level, const char * format, ... );

#define log_trace(fmt, ...)	log_level( LOG_TRACE, fmt, ##__VA_ARGS__ )
#define log_debug(fmt, ...)	log_level( LOG_DEBUG, fmt, ##__VA_ARGS__ )
#define log_info( fmt, ...)	log_level( LOG_INFO , fmt, ##__VA_ARGS__ )
#define log_warn( fmt, ...)	log_level( LOG_WARN , fmt, ##__VA_ARGS__ )
#define log_err(  fmt, ...)	log_level( LOG_ERR  , fmt, ##__VA_ARGS__ )

//! tx data throw uart/itm interface
int log_write( char * data, int len );

//! write implementation for printf
int _write( int file, char * ptr, int len );

//! called before entering in assert
void assert_attention();

//! assert implementation
void __assert_func( const char * filename, int line, const char * assert_func, const char * expr );

//! func, which analys hard fault and
void hard_fault_handler();

#if USE_SPEED_TEST == 1

//! start speed test
void speed_test_start();

//! stop speed test and output to the log
void speed_test_stop();

#endif

#else

#define log_printf(...) 		__NOP()
#define log_level(...) 			__NOP()

#define log_trace(...)
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_err(...)

#define speed_test_start()		__NOP()
#define	speed_test_stop()		__NOP()
#define hard_fault_handler()	__NOP()

#endif

#if USE_DELAY_US == 1

void delay_us( uint16_t delay );

#endif

#ifdef __cplusplus
}
#endif

#endif /* MCU_MOD_MCU_MOD_LOG_H_ */
