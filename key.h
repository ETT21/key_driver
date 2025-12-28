/**
  ******************************************************************************
  * @file       key.h
  * @author     czzzlq
  * @version    V0.1
  * @date       2025年12月23日
  * @brief      按键驱动
  * @attention  仅供学习使用
  * @version    
  *             -V0.1
  * @note       1.WKUP即KEY_UP，因为KEY_UP按键连接到PA0，既可作为普通输入，也可做STM32
  *                 唤醒输入。
  ******************************************************************************
**/
#ifndef __KEY_H__
#define __KEY_H__

#include "../../core/system/system_hal.h"

#define KEY_NUM                         3   //按键数量

//硬件接口
#define KEY0_GPIO_PORT                  GPIOE
#define KEY0_GPIO_PIN                   GPIO_PIN_4
#define KEY0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

#define KEY1_GPIO_PORT                  GPIOE
#define KEY1_GPIO_PIN                   GPIO_PIN_3
#define KEY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

#define WKUP_GPIO_PORT                  GPIOA
#define WKUP_GPIO_PIN                   GPIO_PIN_0
#define WKUP_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define KEY0_READPIN                    HAL_GPIO_ReadPin(KEY0_GPIO_PORT,KEY0_GPIO_PIN)
#define KEY1_READPIN                    HAL_GPIO_ReadPin(KEY1_GPIO_PORT,KEY1_GPIO_PIN)
#define WKUP_READPIN                    HAL_GPIO_ReadPin(WKUP_GPIO_PORT,WKUP_GPIO_PIN)

enum eKeyID{                        //按键ID
    KEY0_ID = 0,
    KEY1_ID,
    WKUP_ID
};

typedef enum{
    KEY_EVENT_NONE = 0,             //无事件
    KEY_EVENT_PRESS,                //按键按下事件
    KEY_EVENT_RELEASE,              //按键释放事件
    KEY_EVENT_LONG_PRESS_TRIGGER    //按键长按事件
} eKeyEvent;

typedef struct{
    GPIO_TypeDef* gpioPort;         //GPIO端口
    uint16_t    gpioPin;            //GPIO引脚
    uint32_t    pressTime;          //按下时间（用于判断长按）
    uint8_t     keyState;           //按键当前状态
    uint8_t     keyEvent;           //触发按键事件,取值为eKeyEvent枚举
    uint8_t     activeLevel;        //按键有效电平
    uint8_t     longPressTrigger;   //长按触发标志
} xKeyCtrlBlock_t;

extern volatile xKeyCtrlBlock_t xKeyCB[KEY_NUM];

void keyInit(void);
void vKeyDetect(xKeyCtrlBlock_t* pvCtrlBlock);
eKeyEvent keyGetEvent(uint8_t keyID);

#endif
