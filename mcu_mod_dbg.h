/*
 *      Author: Alekseev A.R.
 */

#ifndef MCU_MOD_MCU_MOD_DBG_H_
#define MCU_MOD_MCU_MOD_DBG_H_

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

#define SUCC			TERM_GREEN "[SUCC]" TERM_RESET " "
#define INFO			TERM_BLUE "[INFO]" TERM_RESET " "
#define WARN			TERM_YELLOW "[WARN]"  TERM_RESET " "
#define ERR				TERM_RED "[ERR]"  TERM_RESET " "
#define GLOB			"[GLOBAL] "

#if USE_DBG == 1
	int	dbg(const char *format, ...);
	int	dbg_el(const char *format, ...);

	__attribute__((weak)) int dbg_write(char * data, int len);
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

	#define dbg_printf( ... )
	#define dbg_printf_el( ... )

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


#endif /* MCU_MOD_MCU_MOD_DBG_H_ */
