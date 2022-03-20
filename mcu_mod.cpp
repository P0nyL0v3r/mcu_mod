/*
 * mod.cpp
 *      Author: Alekseev Aleksei
 */

/*Include START****************************************************/
#include <mcu_mod.h>
#include "string.h"
#include "assert.h"
#include "stdio.h"
#include "stdarg.h"
/*Include END******************************************************/

/*check core type*/
#include "main.h"
#if	defined STM32F4
	#include "core_cm4.h"
	#include "stm32f4xx.h"
#elif defined STM32F7
	#include "core_cm7.h"
	#include "stm32f7xx.h"
#elif defined STM32G0
	#include "core_cm0plus.h"
	#include "stm32g0xx.h"
#elif defined STM32H7
	#include "core_cm7.h"
	#include "stm32h7xx.h"
#elif defined STM32G4
	#include "core_cm4.h"
	#include "stm32g4xx.h"
#else
	#error "undefined core"
#endif

#if USE_FREERTOS == 1
	#if configGENERATE_RUN_TIME_STATS == 1
		#include "tim.h"
		volatile unsigned long ulHighFrequencyTimerTicks = 0;
		void configureTimerForRunTimeStats(void)
		{
#if defined RUNTIME_TIMER_HAL
			HAL_TIM_Base_Start_IT(&RUNTIME_TIMER_HAL);
#elif defined RUNTIME_TIMER_LL
			LL_TIM_EnableIT_UPDATE(RUNTIME_TIMER_LL);
			LL_TIM_EnableCounter(RUNTIME_TIMER_LL);
#endif
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
	void * __wrap_malloc( size_t xSize ) {
		return pvPortMalloc( xSize );
	}
	void __wrap_free( void* p) {
		vPortFree(p);
	}
	void * __wrap_realloc(void * ptr, size_t size) {
		//вообще я не очень уверен начсет этого, но увидел в одном очень
		//большом и важном приложении, поэтому буду надеяться,
		//что проблем с этим не будет
		void * new_ptr = pvPortMalloc(size);
		memcpy(new_ptr,ptr,size);
		vPortFree(ptr);
		return new_ptr;
	}
	void * _old_realloc(void * ptr, size_t osize,size_t nsize) {
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
		#if !defined(TIM_US) && defined(DWT)
			uint32_t tickstart = DWT->CYCCNT;
			static uint32_t us_tick = SystemCoreClock / 1000000;
			uint32_t wait = delay * us_tick;
		/* Add a freq to guarantee minimum wait */
			if (wait < 0xFFFFFFFFU)	{
				wait += (uint32_t)(us_tick);
			}
			while ((DWT->CYCCNT - tickstart) < wait){}
		#elif defined(TIM_US)
		//Подразумевается, что таймер инкрементируется каждую микросекунду
			uint32_t tickstart = TIM_US->CNT;
			uint32_t wait = delay;
		/* Add a freq to guarantee minimum wait */
			if (wait < 0xFFFFU)	{
				wait += (uint32_t)(1);
			}
			while ((TIM_US->CNT - tickstart) < wait){}
		#else
			#error "must to define us timer"
		#endif
	}
#endif


#if USE_DBG == 1
#if defined DBG_ITF_UART_HAL || defined DBG_ITF_UART_LL
	#include "usart.h"
#endif

	int	dbg (const char *format, ...) {
#if USE_FREERTOS == 1
		portENTER_CRITICAL();
#endif
	    int stat;
	    va_list args;
	    va_start(args, format);

	    stat = vprintf(format, args);

	    va_end(args);

#if USE_FREERTOS == 1
		portEXIT_CRITICAL();
#endif
		return stat;
	}

	int	dbg_el (const char *format, ...) {
#if USE_FREERTOS == 1
		portENTER_CRITICAL();
#endif
	    int stat;
	    va_list args;
	    va_start(args, format);

		stat = vprintf(format, args);

	    va_end(args);

	    dbg("\r\n");

#if USE_FREERTOS == 1
		portEXIT_CRITICAL();
#endif
		return stat;
	}


	int dbg_write(char * data, int len) {
	#if defined ITM
		int DataIdx;
		for (DataIdx = 0; DataIdx < len; DataIdx++)	{
			ITM_SendChar(data[DataIdx]);
		}
	#endif
	#if defined(DBG_ITF_UART_HAL)
		/* Для упрощения алгоримта передачи примитивы ОС не используются.
		 * Вместо этого применяемя активное ожидание, но проверяем, находимся ли
		 * мы в прерывании.
		 */
		HAL_StatusTypeDef stat;
		do {
			stat = HAL_UART_Transmit(&DBG_ITF_UART_HAL, (uint8_t*)data, len, HAL_MAX_DELAY);
		} while(stat != HAL_OK && __get_IPSR() == 0);
	#elif defined(DBG_ITF_UART_LL)
		for(int i = 0; i < len;) {
			if(LL_LPUART_IsActiveFlag_TXE(DBG_ITF_UART_LL)) {
			  while(LL_LPUART_IsActiveFlag_TXE(DBG_ITF_UART_LL)){
				  LL_LPUART_TransmitData8(DBG_ITF_UART_LL, data[i]);
				  i++;
			  }
			}
		}

	#endif
		return len;
	}

	int __io_putchar(int ch) {
		dbg_write((char *)&ch,1);return(ch);
	}
	int _write(int file, char *ptr, int len)	{
		return dbg_write(ptr,len);
	}

	#if USE_SPEED_TEST == 1
		void speed_test_start() {
			dbg_puts(INFO"speed test start");
			//https://stackoverflow.com/questions/36378280/stm32-how-to-enable-dwt-cycle-counter
			CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk;//enable trace
		#if __CORTEX_M	== (7U)
			__DSB(); DWT->LAR = 0xC5ACCE55; __DSB();//added unlock access to DWT (ITM, etc.)registers
			DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;//disable DWT cycle counter
		#endif
			DWT->CYCCNT = 0;//clear DWT cycle counter
			DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk;//enable DWT cycle counter

		}
		void speed_test_stop() {
			uint32_t clk = DWT->CYCCNT;
			uint32_t us = clk/(SystemCoreClock/1000000);
			uint32_t ms = us/1000;
			uint32_t val;

			char * suf = NULL;

			if(clk < SystemCoreClock/100000) {
				suf = (char*)"clk";
				val = clk;
			} else if (us < 10000) {
				suf = (char*)"us";
				val = us;
			} else {
				suf = (char*)"ms";
				val = ms;
			}
			dbg_printf_el(INFO"speed:%lu %s",val,suf);
		}
	#endif

	__weak void assert_attention() {
		dbg_el(ERR"attention");
	}

	void __assert_func( const char *filename, int line, const char *assert_func, const char *expr ) {
		//привлекаем внимание
		assert_attention();
	    __disable_irq();//отключаем прерывания, что бы ничего не вызывалось
	    int i = 0, j = 0;
	    for(;;i++) {
	    	if(i == 0) {
	        	if(j%50 == 0) {
	        		//пишем в консольку
	        		dbg(TERM_RED);

	        		dbg_el("ASSERT file: %s",filename);

	        		dbg_el("line: %d",line);

	        		dbg_el("code: %s",expr);

	        		dbg_el("func: %s",assert_func);

	        		dbg(TERM_RESET);
	        	}
	    	} else if(i >= (int)(SystemCoreClock/100)) {//типо задержка, только без прерываний
	    		i = -1;
	    		j++;
	    	}
	    }
	}
	void usage_fault_handler(uint32_t CFSRValue) {
		char * type;
	   //CFSRValue >>= 16;                  // right shift to lsb
	   if((CFSRValue & (1 << SCB_CFSR_DIVBYZERO_Pos)) != 0) {
		   type = (char*)"Divide by zero";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_UNALIGNED_Pos)) != 0) {
		   type = (char*)"Unaligned access";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_NOCP_Pos)) != 0) {
		   type = (char*)"No coprocessor";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_INVPC_Pos)) != 0) {
		   type = (char*)"Invalid PC load";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_INVSTATE_Pos)) != 0) {
		   type = (char*)"Invalid state";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_UNDEFINSTR_Pos)) != 0) {
		   type = (char*)"Undefined instruction";
	   } else {
		   type = (char*)"undefined";
	   }
		dbg_el("Usage fault: %s",type);
	}

	void bus_fault_handler(uint32_t CFSRValue) {
		char * type;
	   //CFSRValue >>= 8;                  // right shift to lsb
	   if((CFSRValue & (1 << SCB_CFSR_IMPRECISERR_Pos)) != 0) {
		   type = (char*)"Imprecise data bus error";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_PRECISERR_Pos)) != 0) {
		   type = (char*)"Precise data bus error";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_IBUSERR_Pos)) != 0) {
		   type = (char*)"Instruction bus error";
	   } else {
		   type = (char*)"undefined";
	   }
		dbg_el("Bus fault: %s",type);
	}

	void mem_fault_handler(uint32_t CFSRValue) {
		char * type;
	   //CFSRValue >>= 0;                  // right shift to lsb
	   if((CFSRValue & (1 << SCB_CFSR_DACCVIOL_Pos)) != 0) {
		   type = (char*)"Data access violation";
	   }
	   else if((CFSRValue & (1 << SCB_CFSR_IACCVIOL_Pos)) != 0) {
		   type = (char*)"Instruction access violation";
	   } else {
		   type = (char*)"undefined";
	   }
		dbg_el("Memory management fault: %s",type);
	}
	void hard_fault_handler() {
		//https://blog.feabhas.com/2013/02/developing-a-generic-hard-fault-handler-for-arm-cortex-m3cortex-m4/
		dbg_el(ERR"In Hard Fault Handler");
		dbg_el(ERR"SCB->HFSR = 0x%08lX", (uint32_t)SCB->HFSR);
		if(SCB->HFSR & 0x80000000) {
			dbg_el(ERR"debug event");
		}
	    if((SCB->CFSR & 0xFFFF0000) != 0) {
	    	usage_fault_handler(SCB->CFSR);
	    }
	    if((SCB->CFSR & 0x0000FF00) != 0) {
	    	bus_fault_handler(SCB->CFSR);
	    }
	    if((SCB->CFSR & 0x000000FF) != 0) {
	    	mem_fault_handler(SCB->CFSR);
	    }
	    assert("hard fault");
	}
#else
	void _putchar(char ch) {(void)ch;}
#endif
