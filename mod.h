/*
 * mod.h
 *
 *  Created on: 19 апр. 2021 г.
 *      Author: Alekseev Aleksei
 * Description:
 */
#ifndef MOD_HPP_
#define MOD_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/*Include START****************************************************/
#include "stm32f745xx.h"
#include "conf/conf_all.h"
/*Include END******************************************************/

/*FreeRTOS START***************************************************/
#if USE_FREERTOS == 1
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "cmsis_os2.h"
/*Диспетчер ресурсов*/
#if configGENERATE_RUN_TIME_STATS == 1
	extern volatile unsigned long ulHighFrequencyTimerTicks;
//Список дел:
//	* При смене прошивки преобразовать переменную(в случае использования оптимизации):
		//ulTotalRunTime -> volatile ulTotalRunTime
//	* Установить в FreeRTOSConfig.h:
		//#define configGENERATE_RUN_TIME_STATS            1
		//#define configUSE_TRACE_FACILITY                 1
		//#define configUSE_STATS_FORMATTING_FUNCTIONS     1
////////////////	* Добавить в FreeRTOSConfig.h
////////////////		#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS configureTimerForRunTimeStats
////////////////		#define portGET_RUN_TIME_COUNTER_VALUE getRunTimeCounterValue
//	* Скорректировать определение таймера для счета:
		#define RUNTIME_TIMER htim7
//	* Добавить инкрементирование счетчика в обработчике таймера
		/*
		void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
		{
		  if (htim->Instance == *Структура таймера*) {
			  ulHighFrequencyTimerTicks++;
		  }
		}
		 */
#endif
/*Управление кучей*/
	void *pvPortAddElem(void * ptr,size_t one_elem_size, size_t elem_count);
	void _free( void* p);
	void * _malloc( size_t xSize );
	void * _realloc(void * ptr, size_t osize,size_t nsize);
#endif
/*FreeRTOS END***************************************************/

/*stdout START***************************************************/
	int __io_putchar(int ch);
	void _putchar(char ch);
/*собсный формавт*/
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
/*stdout END*****************************************************/

/*DELAY_US START*************************************************/
#if USE_DELAY_US == 1
	void delay_us(uint16_t);
#endif
/*DELAY_US END***************************************************/

/*SPEED_TEST START*************************************************/
#if USE_SPEED_TEST == 1
	void speed_test_start();
	void speed_test_stop();
#endif
/*SPEED_TEST END*************************************************/

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_MOD_HPP_ */
