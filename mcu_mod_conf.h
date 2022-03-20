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

/*
 * us задержка
 */
#ifndef USE_DELAY_US
	#define		USE_DELAY_US	0
#endif

/*
 * Использование маленькой printf библиотеки
 */
#ifndef USE_CUSTOM_STDIO
	#define USE_CUSTOM_STDIO	0
#endif

/* использование rtos API
 * Список дел:
		* Указать линкеру, использовать обертки:
		-Wl,--wrap=malloc
		-Wl,--wrap=free
		-Wl,--wrap=realloc
*/
#ifndef USE_FREERTOS
	#define		USE_FREERTOS	0
#endif
#if USE_FREERTOS == 1
	#include "FreeRTOSConfig.h"
#endif

/* Проверка времени выполнение задач
 * Список дел:
	* При смене прошивки преобразовать переменную(в случае использования оптимизации -oSize):
		ulTotalRunTime -> volatile ulTotalRunTime
	* Установить в FreeRTOSConfig.h:
		#define configGENERATE_RUN_TIME_STATS            1
		#define configUSE_TRACE_FACILITY                 1
		#define configUSE_STATS_FORMATTING_FUNCTIONS     1
	* Добавить в FreeRTOSConfig.h
		#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS configureTimerForRunTimeStats
		#define portGET_RUN_TIME_COUNTER_VALUE getRunTimeCounterValue
	* HAL
		* #define RUNTIME_TIMER_HAL htim*
		* Добавить инкрементирование счетчика в обработчике таймера
			void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
			{...
			  if (htim->Instance == RUNTIME_TIMER.Instance) {
				  ulHighFrequencyTimerTicks++;
			  }
			...}
	* LL
		* #define RUNTIME_TIMER_LL TIM*
		* Добавить в обработчике прерывания
		void TIM*_IRQHandler(void)
		{
			if(LL_TIM_IsActiveFlag_UPDATE(RUNTIME_TIMER_LL)) {
				  LL_TIM_ClearFlag_UPDATE(RUNTIME_TIMER_LL);
				  ulHighFrequencyTimerTicks++;
			} else {
				assert(0 && "undefined tim irq");
			}
		}
*/
#if USE_FREERTOS == 1 && configGENERATE_RUN_TIME_STATS != 1
	#pragma message "configGENERATE_RUN_TIME_STATS == 0"
#endif

#if configGENERATE_RUN_TIME_STATS == 1 && (!defined RUNTIME_TIMER_HAL && !defined RUNTIME_TIMER_LL )
	#warning "define run time stats"
#endif
/*
 * Отслеживание использования стека задач
 * Список дел:
 	 * Установить параметр:
 	 	 configRECORD_STACK_HIGH_ADDRESS	1
 */
#if USE_FREERTOS == 1 && configRECORD_STACK_HIGH_ADDRESS != 1
	#pragma message "configRECORD_STACK_HIGH_ADDRESS == 0"
#endif

/*
 * Перехват переполнения стека
 * Список дел:
 	 * Установить параметр:
 	 	 configRECORD_STACK_HIGH_ADDRESS	1
 	 * Объявить функцию void vApplicationStackOverflowHook(void) {}
 */
#if USE_FREERTOS == 1 && configCHECK_FOR_STACK_OVERFLOW == 0
	#pragma message "configCHECK_FOR_STACK_OVERFLOW == 0"
#endif

/*
 * Перехват нехватки кучи
 * Список дел:
 	 * Установить параметр:
 	 	 configUSE_MALLOC_FAILED_HOOK	1
 	 * Объявить функцию void vApplicationMallocFailedHook(void) {}
 */
#if USE_FREERTOS == 1 && configUSE_MALLOC_FAILED_HOOK != 1
	#pragma message "configUSE_MALLOC_FAILED_HOOK == 0"
#endif


#endif /*MCU_MOD_CONF_H_*/
