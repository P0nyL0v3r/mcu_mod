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

#include "mcu_mod_conf.h"
#include "conf.h"

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
	void __wrap_free( void* p);
	void * __wrap_malloc( size_t xSize );
	void * __wrap_realloc(void * ptr, size_t size);
	void * _old_realloc(void * ptr, size_t osize,size_t nsize);

	/*задержки
	 * стандартный portTICK_PERIOD_MS не корректно работает, когда квант
	 * времени < 1ms. Для этого добавляю свой расчет для задержки*/
	#define stick(s)   ((float)s /(1./(float)configTICK_RATE_HZ))
	#define mstick(ms) ((float)ms/(1000./(float)configTICK_RATE_HZ))
	#define ustick(us) ((float)us/(1000000./(float)configTICK_RATE_HZ))

	typedef enum {
	  rtosPriorityIdle         = tskIDLE_PRIORITY,          ///< priority: idle (lowest)
	  rtosPriorityLow          ,          ///< priority: low
	  rtosPriorityBelowNormal  ,          ///< priority: below normal
	  rtosPriorityNormal       ,          ///< priority: normal (default)
	  rtosPriorityAboveNormal  ,          ///< priority: above normal
	  rtosPriorityHigh         ,          ///< priority: high
	  rtosPriorityRealtime     = configMAX_PRIORITIES -1,          ///< priority: realtime (highest)
	}rtosPrio;
#endif

#if USE_DBG == 1
	#define ENT_DBG_STAT()	__ASM volatile("BKPT #01")

	//теги для отладки

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


	int	dbg(const char *format, ...);
	int	dbg_el(const char *format, ...);

	__attribute__((weak)) int dbg_write(char * data, int len);
	int _write(int file, char *ptr, int len);
	int __io_putchar(int ch);

	//функция для привлечения внимания(помиргать светодиодом и т.д.) перед входом в assert
    void assert_attention();

	void __assert_func( const char *filename, int line, const char *assert_func, const char *expr );

	//обработчик hard fault
	void hard_fault_handler();

	#if USE_SPEED_TEST == 1
		void speed_test_start();
		void speed_test_stop();
	#endif
#else
	#define dbg_puts()
	#define dbg_printf( ... )
	#define dbg_printf_el( ... )

	#define ENT_DBG_STAT()			__NOP()
	#define speed_test_start()		__NOP()
	#define	speed_test_stop()		__NOP()
	#define hard_fault_handler()	__NOP()
#endif

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
