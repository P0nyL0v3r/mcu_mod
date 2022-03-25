# mcu_mod

library that simplifies the deployment of the STM32 project

## mcu_mod_dbg 

* output of messages to the serial port
* changed assert message format
* added hard fault handler with output type of fault

## mcu_mod_rtos 

* integrating the RTOS into system calls
* enumed priorities

## tips and tricks

т.к. библиотека встраивается во все файлы, неплохо было бы записывать здесь все типсы и триксы для облегчения процесса разработки.

library was created for simplify programming, that's why it's will by nice to add some implicit information about STM32.

### optimization

```c++
/*optimization parameter for single function*/
__attribute__((optimize("-Ofast"))) void func(void) {}
```

### ADC

#### Modes

##### starting conversions by timer and writing channels value by DMA

config DMA channel:

```c++
...
hdma.Init.Mode = DMA_CIRCULAR;
...
HAL_NVIC_SetPriority(DMA*_Stream*_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(DMA*_Stream*_IRQn);
```

config ADC:

!Enabling ADC IRQ is not needed. End up of conversion is handling by DMA. 

```c++
  ...
  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  ...
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = *channels num*;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = *req source*;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  ...
```

Enable ADC and event source:

! Enabling ADC IRQ is not needed. Timer update event is enough.

```c++
  HAL_ADC_Start_DMA(&hadc, adc_data, ADC_CHAN_CNT);
  HAL_TIM_Base_Start(&htim);
```

Data can be read from callback:

```c++
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) 
```

АЦП будет сам перезаписывать данные.  Добавлять HAL_ADC_Start_DMA в обработчики прерываний не надо.

example on github:

https://github.com/bartslinger/solar_module_f3/tree/b6843d6f7e75d336eacd3aacae1f4d017883914a

### Debug support for timers, RTC, watchdog, BxCAN and I2C

Для отладки с периферией, требовательной к временным задержкам можно замораживать эту периферию, уточнение на 5.1:

https://www.st.com/resource/en/application_note/dm00354244-stm32-microcontroller-debug-toolbox-stmicroelectronics.pdf#page=51

