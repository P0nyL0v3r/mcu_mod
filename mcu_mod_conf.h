/*
 * mcu_mod_conf.h
 *      Author: Alekseev Aleksei
 * Description:
 */
#ifndef MCU_MOD_CONF_H_
#define MCU_MOD_CONF_H_

//интерфейс отладки
#ifndef USE_DBG
	#define		USE_DBG			1
#endif

#if USE_DBG == 1 && !defined DBG_ITF_UART
#error "must to define debug uart DBG_ITF_UART"
#endif

//использование rtos API
#ifndef USE_FREERTOS
	#define		USE_FREERTOS	0
#endif

//задержка 
#ifndef USE_DELAY_US
	#define		USE_DELAY_US	0
#endif

//тест скорости выполнения кода
#ifndef USE_SPEED_TEST
	#define		USE_SPEED_TEST	0
#endif

#endif /*MCU_MOD_CONF_H_*/
