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


/* Настройки для удобной отладки
 * включаем нормальную работу отладчика и
 * отлючаем собачку в debug режиме*/
#define EN_DEBUG_CORTEX_CM7()	do{	\
	  DBGMCU->APB1FZ = 0xFFFFFFFF;	\
	  DBGMCU->APB2FZ = 0xFFFFFFFF;	\
	  __HAL_DBGMCU_FREEZE_IWDG();	\
}while()

#if USE_FREERTOS == 1
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
	#include "semphr.h"

	#include "cmsis_os.h"

	/*Диспетчер ресурсов*/
	#if configGENERATE_RUN_TIME_STATS == 1
		extern volatile unsigned long ulHighFrequencyTimerTicks;
/*	Список дел:
		* При смене прошивки преобразовать переменную(в случае использования оптимизации):
			ulTotalRunTime -> volatile ulTotalRunTime
		* Установить в FreeRTOSConfig.h:
			#define configGENERATE_RUN_TIME_STATS            1
			#define configUSE_TRACE_FACILITY                 1
			#define configUSE_STATS_FORMATTING_FUNCTIONS     1
		* Добавить в FreeRTOSConfig.h
			#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS configureTimerForRunTimeStats
			#define portGET_RUN_TIME_COUNTER_VALUE getRunTimeCounterValue
		* Скорректировать определение таймера для счета:
			#define RUNTIME_TIMER htim7
		* Добавить инкрементирование счетчика в обработчике таймера
			void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
			{...
			  if (htim->Instance == RUNTIME_TIMER.Instance) {
				  ulHighFrequencyTimerTicks++;
			  }
			...}
*/
	#endif
	
	/*Управление кучей*/
/*	Список дел:
		* В ходе работы было замечено, что если функция malloc определена
			здесь, то не все части программы её могут использовать ->
			некоторые участки кода будут использовать newlib malloc.
			что бы это предотвратить следует вписать следующие строчки
			в main.c :
		void * malloc(size_t xSize) {return _malloc(xSize);}
		void free(void * p) {_free(p);}
		void * realloc(void * ptr, size_t xSize) {assert(0);}
*/
	void *pvPortAddElem(void * ptr,size_t one_elem_size, size_t elem_count);

	void _free( void* p);
	void * _malloc( size_t xSize );
	void * _realloc(void * ptr, size_t osize,size_t nsize);

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
#ifndef assert_attract_attention
	#define assert_attract_attention(...)
#endif

	void __assert_func( const char *filename, int line, const char *assert_func, const char *expr );

	void hard_fault_handler();

#else
	#define  dbg( ... )
#endif

#if USE_DELAY_US == 1
	void delay_us(uint16_t);
#endif

#if USE_SPEED_TEST == 1
	void speed_test_start();
	void speed_test_stop();
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
