/**
  ******************************************************************************
  * @file       key.c
  * @author     czzzlq
  * @version    V0.1
  * @date       2025年12月23日
  * @brief      按键驱动
  * @attention  仅供学习使用
  * @version    
  *             -V0.1
  ******************************************************************************
**/
#include "key.h"

#define DEBOUNCE_TIME           10          //消抖时间
#define KEY_LONG_PRESS_TIME     3000        //按键长按阈值

/* 按键全局控制块 */
volatile xKeyCtrlBlock_t xKeyCB[KEY_NUM] = {
    {KEY0_GPIO_PORT,KEY0_GPIO_PIN,0,0,KEY_EVENT_NONE,0,0},
    {KEY1_GPIO_PORT,KEY1_GPIO_PIN,0,0,KEY_EVENT_NONE,0,0},
    {WKUP_GPIO_PORT,WKUP_GPIO_PIN,0,0,KEY_EVENT_NONE,1,0}
};

/**
 * @brief   按键检测函数
 * @param   pvCtrlBlock：按键控制块指针 
 * @retval  NULL
 */
void vKeyDetect(xKeyCtrlBlock_t* pvCtrlBlock){
    GPIO_PinState pinLevel = HAL_GPIO_ReadPin(pvCtrlBlock->gpioPort,pvCtrlBlock->gpioPin);

    if(pinLevel == (pvCtrlBlock->activeLevel)){  //按键按下
        if(pvCtrlBlock->keyState != 1){  //按键从释放到按下
            pvCtrlBlock->pressTime = HAL_GetTick(); //记录按下时的Tick值
            pvCtrlBlock->keyState = 1;
            pvCtrlBlock->longPressTrigger = 0;   //重置长按标志
        }
        if(pvCtrlBlock->keyState == 1){
            uint32_t pressDuration = HAL_GetTick() - (pvCtrlBlock->pressTime);  //获取按键按下的持续时间

            if(pressDuration >= DEBOUNCE_TIME){   //消抖完成
                if((pressDuration >= KEY_LONG_PRESS_TIME) && (!pvCtrlBlock->longPressTrigger)){
                    pvCtrlBlock->keyEvent = KEY_EVENT_LONG_PRESS_TRIGGER;    //触发长按事件
                    pvCtrlBlock->longPressTrigger = 1;
                }
                //短按等释放后判断
            }
        }
    }else{
        if(pvCtrlBlock->keyState == 1){ //从按下到释放
            uint32_t pressDuration = HAL_GetTick() - (pvCtrlBlock->pressTime);  //获取按键按下的持续时间

            if(pressDuration >= DEBOUNCE_TIME){
                if((pressDuration < KEY_LONG_PRESS_TIME) && (!pvCtrlBlock->longPressTrigger)){  //短按的按键在长按状态下不会做出反应
                    pvCtrlBlock->keyEvent = KEY_EVENT_PRESS;
                }
            }
        }
        //标志重置
        pvCtrlBlock->keyState = 0;
        pvCtrlBlock->longPressTrigger = 0;
    }
    
}

/**
 * @brief   按键检测函数
 * @param   NULL
 * @retval  NULL
 */
void keyInit(void){
    KEY0_GPIO_CLK_ENABLE();
    KEY1_GPIO_CLK_ENABLE();
    WKUP_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    //KEY0
    GPIO_InitStructure.Pin = KEY0_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY0_GPIO_PORT,&GPIO_InitStructure);

    //KEY1
    GPIO_InitStructure.Pin = KEY1_GPIO_PIN;
    HAL_GPIO_Init(KEY1_GPIO_PORT,&GPIO_InitStructure);

    //WKUP
    GPIO_InitStructure.Pin = WKUP_GPIO_PIN;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(WKUP_GPIO_PORT,&GPIO_InitStructure);
}

/**
 * @brief 按键事件获取
 * @param keyID 
 * @return eKeyEvent 
 */
eKeyEvent keyGetEvent(uint8_t keyID){
    __disable_irq();    //关闭中断
    eKeyEvent event = xKeyCB[keyID].keyEvent;
    xKeyCB[keyID].keyEvent = KEY_EVENT_NONE;
    __enable_irq();     //开启中断
    return event;
}
