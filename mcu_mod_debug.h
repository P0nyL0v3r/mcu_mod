#ifndef MCU_MOD_MCU_MOD_DEBUG_H_
#define MCU_MOD_MCU_MOD_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mcu_mod.h"
#include "mcu_mod_conf.h"

#include "stdint.h"

#if USE_LOG == 1

/* Types ---------------------------------------------------------------------*/

/* Macro ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
//! func, which analys hard fault
void print_hard_fault_reason();

//! func, which analys rcc and print reset reason
uint32_t print_reset_reason();

#endif//#if USE_LOG == 1


#if USE_DELAY_US == 1

void delay_us( uint16_t delay );

#endif

#if USE_DWT == 1

// https://mcuoneclipse.com/2017/01/30/cycle-counting-on-arm-cortex-m-with-dwt/
// https://stackoverflow.com/questions/36378280/stm32-how-to-enable-dwt-cycle-counter
void dwt_init();
void dwt_enable();
void dwt_disable();
void dwt_reset_counter();
uint32_t dwt_get_counter();

#endif

#ifdef __cplusplus
}
#endif

#endif /* MCU_MOD_MCU_MOD_DEBUG_H_ */
