//
// Created by QinYouzhuo on 2025/1/12.
//

#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "stm32f1xx.h"
#include "./LED/Bsp_LED.h"

#define User_Key1_Key2_Init  do{Key_Init(&User_Key1, GPIOA, GPIO_PIN_0, Key1_Click_Callback, Key1_Double_Callback,Key1_Long_Callback);Key_Init(&User_Key2, GPIOC, GPIO_PIN_13, Key2_Click_Callback, Key2_Double_Callback,Key2_Long_Callback);}while(0)


typedef enum {
    KEY_STATE_IDLE,
    KEY_STATE_DEBOUNCE,
    KEY_STATE_WAIT_RELEASE,
    KEY_STATE_WAIT_DOUBLE,
    KEY_STATE_LONG_WAIT_RELEASE
} KeyState;

typedef enum {
    KEY_EVENT_NONE,
    KEY_EVENT_CLICK,
    KEY_EVENT_DOUBLE_CLICK,
    KEY_EVENT_LONG_PRESS
} KeyEvent;// 按键触发的事件

typedef void (*KeyCallback)(void);

/* 按键状态机内部使用结构体 */
typedef struct {
    KeyState state;          // 当前状态
    KeyState prev_state;     // 前一个状态（用于调试）
    KeyEvent last_event;     // 最近一次触发的事件
    uint32_t state_entry;    // 状态进入时间戳
    uint32_t click_counter; // 按键计数器
    GPIO_TypeDef *port;
    uint16_t pin;
    KeyCallback click_cb;
    KeyCallback double_cb;
    KeyCallback long_cb;
} KeyHandle;

extern KeyHandle User_Key1, User_Key2;

/**
 * @brief  按键GPIO初始化
 * @note    根据实际硬件配置
 * */
void KEY_GPIO_Init(void);

// 函数声明

/**
  * @brief  按键初始化
  * @param  key 按键句柄指针
  * @param  port GPIO端口
  * @param  pin GPIO引脚
  * @param  click_cb 单击回调（可为NULL）
  * @param  double_cb 双击回调（可为NULL）
  * @param  long_cb 长按回调（可为NULL）
  */
void Key_Init(KeyHandle *key, GPIO_TypeDef *port, uint16_t pin,
              KeyCallback click_cb, KeyCallback double_cb, KeyCallback long_cb);

/**
  * @brief  按键状态机处理（需周期性调用）
  * @param  key 按键句柄指针
  */
void Key_Process(KeyHandle *key);

KeyEvent Key_GetLastEvent(KeyHandle *key);

//User_Key1回调函数
//用户自定义
void Key1_Click_Callback(void);

void Key1_Double_Callback(void);

void Key1_Long_Callback(void);

//User_Key2回调函数
void Key2_Click_Callback(void);

void Key2_Double_Callback(void);

void Key2_Long_Callback(void);

#endif //_BSP_KEY_H
