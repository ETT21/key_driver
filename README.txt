使用说明：

1.需要再定时器中断回调函数中调用vKeyDetect()用于检测按键

以STM32F103的TIM5为例（假设已经初始化的TIM5）

```
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)

{

  if(htim->Instance == TIM5 ){

​    for(uint8_t i = 0; i < KEY_NUM; i++){

​      vKeyDetect(&xKeyCB[i]);

​    }

  }

}
```

2.用于按键检测的定时器应设置为10ms中断一次，用于消抖

 * ```
 /**
 
  * @brief       基本定时器TIM5定时中断初始化函数
    
     * @note
    
     * 基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
    
     * 基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
    
     * 定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
    
     * Ft=定时器工作频率,单位:Mhz
        *
    
     * @param       arr: 自动重装值。
    
  * @param       psc: 时钟预分频数
 
  * @retval      无
    */
    void btim_tim5_init(uint16_t arr, uint16_t psc)
    {
    BTIM_TIM5_INT_CLK_ENABLE();                                                 /* 使能TIMx时钟 */
 
    g_tim5_handle.Instance = BTIM_TIM5_INT;                                     /* 通用定时器x */
    g_tim5_handle.Init.Prescaler = psc;                                         /* 分频 */
    g_tim5_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                        /* 向上计数器 */
    g_tim5_handle.Init.Period = arr;                                            /* 自动装载值 */
    g_tim5_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                  /* 时钟分频因子 */
    HAL_TIM_Base_Init(&g_tim5_handle);
 
    //TIM5中断受FreeRTOS管理
    HAL_NVIC_SetPriority(BTIM_TIM5_INT_IRQn, 6, 0);                             /* 设置中断优先级，抢占优先级4，子优先级0 */
    HAL_NVIC_EnableIRQ(BTIM_TIM5_INT_IRQn);                                     /* 开启ITM3中断 */
 
    HAL_TIM_Base_Start_IT(&g_tim5_handle);                                      /* 使能定时器x和定时器x更新中断 */
    }
 ```
 
 STM32F103ZET6系统时钟为72MHz
 
 ```
 btim_tim5_init(10000-1,72-1);
 ```

3.按键检测示例（以FreeRTOS为例）

```
/**

 * @brief  Task1

 * @param  pvParameters :传入参数

 * @retval  无

 */

void Task1(void *pvParameters)

{

  (void)pvParameters;

  while(1)

  {

​    if(keyGetEvent(KEY0_ID) == KEY_EVENT_PRESS){

​      LED0_TOGGLE();

​    }

​    if(keyGetEvent(KEY1_ID) == KEY_EVENT_PRESS){

​      LED1_TOGGLE();

​    }

​    vTaskDelay(10);

  }

}
```

