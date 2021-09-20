/*
 * mcu_mod_conf.h
 *      Author: Alekseev Aleksei
 * Description:
 */
#ifndef MCU_MOD_CONF_H_
#define MCU_MOD_CONF_H_

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
 * использование таймера для us задержки
 */
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
#if USE_FREERTOS == 1 && configGENERATE_RUN_TIME_STATS != 1
	#warning "загрузка выполнения задач не отслеживается"
#endif

/*
 * Отслеживание использования стека задач
 * Список дел:
 	 * Установить параметр:
 	 	 configRECORD_STACK_HIGH_ADDRESS	1
 */
#if USE_FREERTOS == 1 && configRECORD_STACK_HIGH_ADDRESS != 1
	#warning "стек задач не отслеживается"
#endif

/*
 * Перехват переполнения стека
 * Список дел:
 	 * Установить параметр:
 	 	 configRECORD_STACK_HIGH_ADDRESS	1
 	 * Объявить функцию void vApplicationStackOverflowHook(void) {}
 */
#if USE_FREERTOS == 1 && configCHECK_FOR_STACK_OVERFLOW != 1
	#warning "переполнение стека не перехвачивается"
#endif

/*
 * Перехват нехватки кучи
 * Список дел:
 	 * Установить параметр:
 	 	 configUSE_MALLOC_FAILED_HOOK	1
 	 * Объявить функцию void vApplicationMallocFailedHook(void) {}
 */
#if USE_FREERTOS == 1 && configUSE_MALLOC_FAILED_HOOK != 1
	#warning "Нехватка кучи не перехвачивается"
#endif


#endif /*MCU_MOD_CONF_H_*/
