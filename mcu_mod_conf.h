/*
 * mcu_mod_conf.h
 *      Author: Alekseev Aleksei
 * Description:
 */
#ifndef MCU_MOD_CONF_H_
#define MCU_MOD_CONF_H_

#include "conf.h"

//интерфейс отладки
#ifndef USE_DBG
	#define		USE_DBG			0
#endif

/* тест скорости выполнения кода
 * Список дел:
 * Включить USE_DBG
 */
#ifndef USE_SPEED_TEST
	#define		USE_SPEED_TEST	0
#endif

//us задержка
#ifndef USE_DELAY_US
	#define		USE_DELAY_US	0
#endif

//использование таймера для us задержки
#if USE_DELAY_US == 1 && !defined DWT && !defined TIM_US
	#error "определи микросекундный таймер для задержки"
#endif

/* использование rtos API
 * Список дел:
		* В ходе работы было замечено, что если функция malloc определена
			здесь, то не все части программы её могут использовать ->
			некоторые участки кода будут использовать newlib malloc.
			что бы это предотвратить следует вписать следующие строчки
			в main.c :
		void * malloc(size_t xSize) {return _malloc(xSize);}
		void free(void * p) {_free(p);}
		void * realloc(void * ptr, size_t xSize) {return _realloc(ptr,xSize);}
*/
#ifndef USE_FREERTOS
	#define		USE_FREERTOS	0
#endif


/* Проверка времени выполнение задач
 * Список дел:
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
#if configGENERATE_RUN_TIME_STATS == 1 && !defined RUNTIME_TIMER
	#error "must to define freertos runtime stat timer"
#endif

#endif /*MCU_MOD_CONF_H_*/
