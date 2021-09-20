/*
 * mcu_mod.h
 *      Author: Alekseev Aleksei
 * Description:
 */
#ifndef MCU_MOD_H_
#define MCU_MOD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "conf.h"
#include <mcu_mod_conf.h>

#if USE_FREERTOS == 1
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
	#include "semphr.h"

	#include "cmsis_os.h"

	/*Диспетчер ресурсов*/
	#if configGENERATE_RUN_TIME_STATS == 1
		extern volatile unsigned long ulHighFrequencyTimerTicks;
	#endif
	
	/*Управление кучей*/
	void *pvPortAddElem(void * ptr,size_t one_elem_size, size_t elem_count);
	void _free( void* p);
	void * _malloc( size_t xSize );
	void * _realloc(void * ptr, size_t size);
	void * _old_realloc(void * ptr, size_t osize,size_t nsize);

	/*задержки
	 * стандартный portTICK_PERIOD_MS не корректно работает, когда квант
	 * времени < 1ms. Для этого добавляю свой расчет для задержки*/
	#define stick(s)   ((float)s /(1./(float)configTICK_RATE_HZ))
	#define mstick(ms) ((float)ms/(1000./(float)configTICK_RATE_HZ))
	#define ustick(us) ((float)us/(1000000./(float)configTICK_RATE_HZ))
#endif

#if USE_DBG == 1
	#define ENT_DBG_STAT()	__ASM volatile("BKPT #01")

	//теги для отладки
	#define TERM_GREEN		"\033[0;32m"
	#define TERM_YELLOW		"\033[0;33m"
	#define TERM_RED		"\033[1;31m"
	#define TERM_RESET		"\033[0;0m"

	#define INFO			TERM_GREEN "[INFO]" TERM_RESET " "
	#define WARN			TERM_YELLOW "[WARN]"  TERM_RESET " "
	#define ERR				TERM_RED "[ERR]"  TERM_RESET " "
	#define GLOB			"[GLOBAL] "

	#include "stdio.h"
	#define dbg_endl()		printf("\r\n")
	#define dbg( args ... ) printf(args); dbg_endl()

	int __io_putchar(int ch);
	void _putchar(char ch);

	//функция для привлечения внимания(помиргать светодиодом и т.д.) при входе в assert
    void assert_attention();

	void __assert_func( const char *filename, int line, const char *assert_func, const char *expr );

	//включить режим отладки
	void en_dbg();

	#if USE_SPEED_TEST == 1
		void speed_test_start();
		void speed_test_stop();
	#endif
#else
	#define  dbg( ... )
	#define  en_dbg( ... )
	#define  speed_test_start()
	#define	speed_test_stop()
#endif

	//обработчик hard fault
	void hard_fault_handler();

#if USE_DELAY_US == 1
	void delay_us(uint16_t);
#endif

//additional
	//байт в строку
	#define B2B_FORM "%c%c%c%c%c%c%c%c"
	#define B2B(byte)  \
	  (byte & 0x80 ? '1' : '0'), \
	  (byte & 0x40 ? '1' : '0'), \
	  (byte & 0x20 ? '1' : '0'), \
	  (byte & 0x10 ? '1' : '0'), \
	  (byte & 0x08 ? '1' : '0'), \
	  (byte & 0x04 ? '1' : '0'), \
	  (byte & 0x02 ? '1' : '0'), \
	  (byte & 0x01 ? '1' : '0')
	  
#ifdef __cplusplus
}
#endif

#endif /*MCU_MOD_H_ */
