### Оптимизация

```c++
/*установить параметр оптимизации для отдельно взятой функции*/
__attribute__((optimize("-Ofast"))) void func(void) {}
```

### АЦП

Чтобы использовать АЦП в режиме:

* старт преобразований от таймера и запись значений каналов в память через DMA, нужно:
  настроить DMA канал:

  ```c++
  ...
  hdma.Init.Mode = DMA_CIRCULAR;
  ...
  ```

  настроить АЦП:

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

  Включить АЦП и источник запроса

  ```c++
    HAL_ADC_Start_DMA(&hadc, adc_data, ADC_CHAN_CNT);
    HAL_TIM_Base_Start_IT(&htim);
  ```

  
