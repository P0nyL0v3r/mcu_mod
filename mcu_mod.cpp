/*
 * mod.cpp
 *      Author: Alekseev Aleksei
 */

/*Include START****************************************************/
#include <mcu_mod.h>
#include "string.h"
#include "assert.h"
/*Include END******************************************************/

/*check core type*/
#include "main.h"
#if	defined(STM32F4)
	#include "core_cm4.h"
#elif defined(STM32F7) || defined(STM32H7)
	#include "core_cm7.h"
#else
	#error "undefined core"
#endif

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

	void __assert_func( const char *filename, int line, const char *assert_func, const char *expr ) {
	    __disable_irq();//отключаем прерывания, что бы ничего не вызывалось
	    int i = 0, j = 0;
	    for(;;i++) {
	    	if(i == 0) {
	    		assert_attract_attention();
	        	if(j%50 == 0)printf(TERM_RED "ASSERT file: %s\r\n line: %d\r\n code: %s\r\n func: %s\r\n" TERM_RESET, filename, line, expr, assert_func);//пишем в консольку
	    	} else if(i >= (int)(SystemCoreClock/100)) {//типо задержка, только без прерываний
	    		i = -1;
	    		j++;
	    	}
	    }
	}

	void usage_fault_handler(uint32_t CFSRValue) {
	   dbg("Usage fault: ");
	   //CFSRValue >>= 16;                  // right shift to lsb
	   if((CFSRValue & (1 << SCB_CFSR_DIVBYZERO_Pos)) != 0) {
		   dbg("Divide by zero");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_UNALIGNED_Pos)) != 0) {
		   dbg("Unaligned access");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_NOCP_Pos)) != 0) {
		   dbg("No coprocessor");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_INVPC_Pos)) != 0) {
		   dbg("Invalid PC load");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_INVSTATE_Pos)) != 0) {
		   dbg("Invalid state");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_UNDEFINSTR_Pos)) != 0) {
		   dbg("Undefined instruction");
	   }
	}

	void bus_fault_handler(uint32_t CFSRValue) {
		dbg("Bus fault: ");
	   //CFSRValue >>= 8;                  // right shift to lsb
	   if((CFSRValue & (1 << SCB_CFSR_IMPRECISERR_Pos)) != 0) {
		   dbg("Imprecise data bus error");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_PRECISERR_Pos)) != 0) {
		   dbg("Precise data bus error");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_IBUSERR_Pos)) != 0) {
		   dbg("Instruction bus error");
	   }
	}

	void mem_fault_handler(uint32_t CFSRValue) {
		dbg("Memory management fault: ");
	   //CFSRValue >>= 0;                  // right shift to lsb
	   if((CFSRValue & (1 << SCB_CFSR_DACCVIOL_Pos)) != 0) {
		   dbg("Data access violation");
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_IACCVIOL_Pos)) != 0) {
		   dbg("Instruction access violation");
	   }
	}
	void hard_fault_handler() {
		//https://blog.feabhas.com/2013/02/developing-a-generic-hard-fault-handler-for-arm-cortex-m3cortex-m4/
		static char msg[80];
		dbg(ERR"In Hard Fault Handler");
		sprintf(msg, "SCB->HFSR = 0x%08x", SCB->HFSR);
		dbg(msg);
	    if((SCB->CFSR & 0xFFFF0000) != 0) {
	    	usage_fault_handler(SCB->CFSR);
	    }
	    if((SCB->CFSR & 0x0000FF00) != 0) {
	    	bus_fault_handler(SCB->CFSR);
	    }
	    if((SCB->CFSR & 0x000000FF) != 0) {
	    	mem_fault_handler(SCB->CFSR);
	    }
	    assert(0);
	}

#endif



#if USE_SPEED_TEST == 1
	void speed_test_start() {
		dbg(INFO"speed test start");
	#if __CORTEX_M	== 7
		CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk;
		__DSB(); DWT->LAR = 0xC5ACCE55; __DSB();
		DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk;
	#else
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	#endif
	}
	void speed_test_stop() {
		dbg(INFO"speed:%lu(us)",DWT->CYCCNT/(SystemCoreClock / 1000000));
	}
#endif

