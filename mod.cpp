/*
 * mod.cpp
 *      Author: Alekseev Aleksei
 */

/*Include START****************************************************/
#include "mod.h"
#include "string.h"
#include "assert.h"
/*Include END******************************************************/

#if USE_FREERTOS == 1

	#if configGENERATE_RUN_TIME_STATS == 1
		#include "tim.h"
		volatile unsigned long ulHighFrequencyTimerTicks = 0;
		void configureTimerForRunTimeStats(void)
		{
			HAL_TIM_Base_Start_IT(&RUNTIME_TIMER);
		}
		unsigned long getRunTimeCounterValue(void)
		{
			return ulHighFrequencyTimerTicks;
		}
	#endif

	#ifdef __cplusplus
		void* operator new(size_t sz) {
			return pvPortMalloc(sz);
		}
		void* operator new[](size_t sz) {
			return pvPortMalloc(sz);
		}
		void operator delete(void* p) {
			vPortFree(p);
		}
		void operator delete[](void* p) {
			vPortFree(p);
		}
		void* operator new(size_t size, void* p) {
			 assert(0);
			(void)size;
			return p;
		}
		void* operator new[](size_t size, void* p) {
			assert(0);
			(void)size;
			return p;
		}
		void operator delete(void*, void*) {
			assert(0);
		}
		void operator delete[](void*, void*) {
			assert(0);
		}
	#endif
	void * _malloc( size_t xSize ) {
		return pvPortMalloc( xSize );
	}
	void _free( void* p) {
		vPortFree(p);
	}
	void * _realloc(void * ptr, size_t osize,size_t nsize) {
	   if (nsize == 0) {
		   vPortFree(ptr);
		  return NULL;
	   }
	   else if (!ptr) {
		  return pvPortMalloc(nsize);
	   }
	   else if (nsize <= osize) {
		  return ptr;
	   }
	   else {
		  void *ptrNew = pvPortMalloc(nsize);
		  if (ptrNew)
		  {
			  memcpy(ptrNew, ptr, osize);
			  vPortFree(ptr);
		  }
		  return ptrNew;
		}
	}
	void *pvPortAddElem(void * ptr,size_t one_elem_size, size_t elem_count) {
		if(elem_count == 0)
		{
			ptr = pvPortMalloc(one_elem_size);
			return ptr;
		}
		size_t total_size = one_elem_size * (elem_count+1);
		void * old_ptr = ptr;
		ptr =pvPortMalloc(total_size);
		memcpy(ptr,old_ptr,total_size - one_elem_size);
		vPortFree(old_ptr);
		return ptr;
	}
#endif


#if USE_DELAY_US == 1
	void delay_us(uint16_t delay) {
		#ifdef DWT
			uint32_t tickstart = DWT->CYCCNT;
			static uint32_t us_tick = SystemCoreClock / 1000000;
			uint32_t wait = delay * us_tick;
		/* Add a freq to guarantee minimum wait */
			if (wait < 0xFFFFFFFFU)
			{
				wait += (uint32_t)(us_tick);
			}
			while ((DWT->CYCCNT - tickstart) < wait)
			{
			}
		#else
		//Подразумевается, что таймер инкрементируется каждую микросекунду
		//FIXME: При использовании определить таймер
		#define TIM_CNT //TIM*->CNT
			uint32_t tickstart = TIM_CNT;
			uint32_t wait = delay;
		/* Add a freq to guarantee minimum wait */
			if (wait < 0xFFFFU)	{
				wait += (uint32_t)(1);
			}
			while ((TIM_CNT - tickstart) < wait){}
		#undef TIM_CNT
		#endif
	}
#endif


#if USE_DBG == 1
	#include "usart.h"
	int __io_putchar(int ch) {
	#ifdef ITM
		ITM_SendChar(ch);
	#endif
		HAL_UART_Transmit(&DBG_ITF_UART, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
		return(ch);
	}
	void _putchar(char ch) {
		__io_putchar(ch);
	}
#endif

#if USE_SPEED_TEST == 1
	#define RESET_CYCLE_COUNTER()  do { \
			CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk; \
			__DSB(); DWT->LAR = 0xC5ACCE55; __DSB(); \
			DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; \
			DWT->CYCCNT = 0; \
			DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk; \
		}while(0)
	#define GET_CYCLE_COUNTER(x)                x=DWT->CYCCNT;
	void speed_test_start() {
		dbg(INFO"speed test start");
		RESET_CYCLE_COUNTER();
	}
	void speed_test_stop() {
		dbg(INFO"speed:%lu(us)",DWT->CYCCNT/(SystemCoreClock / 1000000));
	}
#endif

