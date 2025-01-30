//
// Created by QinYouzhuo on 2025/1/12.
//

#include "Bsp_Key.h"


// 时间阈值配置（单位：ms）
#define DEBOUNCE_TIME        20    // 消抖时间
#define LONG_PRESS_TIME      800  // 长按判定时间
#define DOUBLE_CLICK_TIME    300   // 双击间隔时间


/**
 * 定义Key1,Key2句柄
  */
KeyHandle User_Key1, User_Key2;

/**
 * @brief  按键GPIO初始化
 * @note    根据实际硬件配置
 * */
void KEY_GPIO_Init(void)
{
    GPIO_InitTypeDef KEY_GPIO_Init;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    KEY_GPIO_Init.Pin = GPIO_PIN_0;
    KEY_GPIO_Init.Mode = GPIO_MODE_INPUT;
    KEY_GPIO_Init.Pull = GPIO_NOPULL;
    KEY_GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &KEY_GPIO_Init);

    KEY_GPIO_Init.Pin = GPIO_PIN_13;

    HAL_GPIO_Init(GPIOC, &KEY_GPIO_Init);

}

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
              KeyCallback click_cb, KeyCallback double_cb, KeyCallback long_cb)
{
    key->state = KEY_STATE_IDLE;
    key->prev_state = KEY_STATE_IDLE;
    key->last_event = KEY_EVENT_NONE;
    key->port = port;
    key->pin = pin;
    key->click_cb = click_cb;
    key->double_cb = double_cb;
    key->long_cb = long_cb;
    key->click_counter = 0;
    key->state_entry = HAL_GetTick();
}

/**
  * @brief  获取经过的时间（处理Tick溢出）
  * @param  now 当前时间戳
  * @param  prev 先前时间戳
  * @retval 经过的时间（ms）
  */
static inline uint32_t Get_Elapsed(uint32_t now, uint32_t prev)
{
    return (now >= prev) ? (now - prev) : (UINT32_MAX - prev + now + 1);
}

/**
  * @brief  处理状态迁移
  * @param  key 按键句柄
  * @param  new_state 新状态
  */
static void Change_State(KeyHandle *key, KeyState new_state)
{
    key->prev_state = key->state;
    key->state = new_state;
    key->state_entry = HAL_GetTick();
    // key->click_counter = 0;
}

/**
  * @brief  按键状态机处理（需周期性调用）
  * @param  key 按键句柄指针
  */
void Key_Process(KeyHandle *key)
{
    const uint8_t key_active = (HAL_GPIO_ReadPin(key->port, key->pin) == GPIO_PIN_SET);//高电平为按下
    const uint32_t now = HAL_GetTick();

    switch (key->state) {
        case KEY_STATE_IDLE:
            if (key_active) {
                Change_State(key, KEY_STATE_DEBOUNCE);
            }

            break;

        case KEY_STATE_DEBOUNCE:
            if (Get_Elapsed(now, key->state_entry) >= DEBOUNCE_TIME) {
                if (key_active) {
                    Change_State(key, KEY_STATE_WAIT_RELEASE);
                } else {
                    Change_State(key, KEY_STATE_IDLE);
                }
            }
            break;

        case KEY_STATE_WAIT_RELEASE:
            if (!key_active && Get_Elapsed(now, key->state_entry) < LONG_PRESS_TIME) {
                Change_State(key, KEY_STATE_WAIT_DOUBLE);
                key->click_counter++;
            } else if (Get_Elapsed(now, key->state_entry) >= LONG_PRESS_TIME) {
                if (key->long_cb) {
                    key->long_cb();
                    key->last_event = KEY_EVENT_LONG_PRESS;
                }
                Change_State(key, KEY_STATE_LONG_WAIT_RELEASE);
            }
            break;

        case KEY_STATE_WAIT_DOUBLE:
            if (key->click_counter == 1) {
                if (Get_Elapsed(now, key->state_entry) >= DOUBLE_CLICK_TIME) {
                    if (key->click_cb) {
                        key->click_cb();
                        key->last_event = KEY_EVENT_CLICK;
                        key->click_counter = 0;
                    }
                    Change_State(key, KEY_STATE_IDLE);
                } else if (key_active) {
                    Change_State(key, KEY_STATE_DEBOUNCE);
                }
            } else if (key->click_counter == 2) {
                if (key->double_cb) {
                    key->double_cb();
                    key->last_event = KEY_EVENT_DOUBLE_CLICK;
                    key->click_counter = 0;
                }
                Change_State(key, KEY_STATE_IDLE);
            }
            break;

        case KEY_STATE_LONG_WAIT_RELEASE:
            if (!key_active) {
                Change_State(key, KEY_STATE_IDLE);
                key->click_counter = 0;
            }

        default:
            break;
    }
}


__weak void Key1_Click_Callback(void)
{
    // 处理单击事件
    LED_Toggle(LED_RED_Pin);
}

__weak void Key1_Double_Callback()
{
    // 处理双击事件
    LED_Toggle(LED_GREEN_Pin);
}

__weak void Key1_Long_Callback()
{
    // 处理长按事件
    LED_Toggle(LED_BLUE_Pin);
}


__weak void Key2_Click_Callback()
{
    // 处理单击事件
}

__weak void Key2_Double_Callback()
{
    // 处理双击事件
}

__weak void Key2_Long_Callback()
{
    // 处理长按事件
}


