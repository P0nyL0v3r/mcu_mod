/*
 *      Author: Alekseev A.R.
 */

#ifndef MCU_MOD_MCU_MOD_LOG_H_
#define MCU_MOD_MCU_MOD_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_mod.h"

//colored tag for debug
#define TERM_RED		"\033[1;31m"
#define TERM_GREEN		"\033[0;32m"
#define TERM_YELLOW		"\033[0;33m"
#define TERM_BLUE		"\033[0;34m"
#define TERM_RESET		"\033[0;0m"

typedef enum {
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERR
}log_level_t;

#if USE_LOG == 1
	int	log_printf(const char *format, ...);

	int	log_level(log_level_t level,const char *format, ...);

	#define log_trace(...)	log_level(LOG_TRACE, __VA_ARGS__)
	#define log_debug(...)	log_level(LOG_DEBUG, __VA_ARGS__)
	#define log_info(...)	log_level(LOG_INFO, __VA_ARGS__)
	#define log_warn(...)	log_level(LOG_WARN, __VA_ARGS__)
	#define log_err(...)	log_level(LOG_ERR, __VA_ARGS__)

	__attribute__((weak)) int log_write(char * data, int len);

	int _write(int file, char *ptr, int len);
	int __io_putchar(int ch);

    void assert_attention();
	void __assert_func( const char *filename, int line, const char *assert_func, const char *expr );

	void hard_fault_handler();

	#if USE_SPEED_TEST == 1
		void speed_test_start();
		void speed_test_stop();
	#endif

#else

	#define log_printf(...) __NOP()
	#define log_level(...) __NOP()

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
	void delay_us(uint16_t);
#endif


#ifdef __cplusplus
}
#endif


#endif /* MCU_MOD_MCU_MOD_LOG_H_ */
