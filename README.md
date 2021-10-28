# mcu_mod

mcu_mod - библиотека для оптимизации процесса разработки. Включает в себя модули общего назначения.

##### USE_DBG

* Перенаправляет стандартный вывод сообщений (printf/puts/...) в отладочные интерфейсы(ITF/UART) 

##### USE_FREERTOS

* Переопределяет стандартные вызовы, делая их совместимыми с кучей FreeRtos'a

## tips_and_tricks

т.к. библиотека встраивается во все файлы, неплохо было бы записывать здесь все типсы и триксы для облегчения процесса разработки.

### Оптимизация

```c++
/*установить параметр оптимизации для отдельно взятой функции*/
__attribute__((optimize("-Ofast"))) void func(void) {}
```

### АЦП

#### Режимы

* начало преобразований от таймера и запись значений каналов в память через DMA

настроить DMA канал:

```c++
...
hdma.Init.Mode = DMA_CIRCULAR;
...
HAL_NVIC_SetPriority(DMA*_Stream*_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(DMA*_Stream*_IRQn);
```

настроить АЦП:

!Включать прерывания АЦП не обязательно. Окончание передачи обрабатывается прерыванием DMA 

```c++
  ...
  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  ...
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = *кол-во каналов*;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = *источник запроса*;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  ...
```

Включить АЦП и источник запроса:

! Включать прерывания таймера не обязательно. Достаточно обновления события таймера.

```c++
  HAL_ADC_Start_DMA(&hadc, adc_data, ADC_CHAN_CNT);
  HAL_TIM_Base_Start(&htim);
```

Данные можно снимать с переопределяемой функции:

```c++
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) 
```

АЦП будет сам перезаписывать данные.  Добавлять HAL_ADC_Start_DMA в обработчики прерываний не надо.

Пример реализации кода есть на github:

https://github.com/bartslinger/solar_module_f3/tree/b6843d6f7e75d336eacd3aacae1f4d017883914a
