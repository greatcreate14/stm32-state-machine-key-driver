# STM32 按键驱动与状态机

## 概述

该项目为 STM32 微控制器提供了一个按键驱动，使用状态机来处理多种按键事件，包括单击、双击和长按。用户可以通过回调函数定制按键按下后的处理逻辑。

## 特性

- **单击检测**：检测单次按键按下。
- **双击检测**：检测在短时间内的两次连续按键按下。
- **长按检测**：检测按键按下超过指定时间的事件。
- **消抖处理**：实现按键消抖，避免误触发。
- **可自定义回调函数**：允许用户为单击、双击和长按事件定义自定义处理函数。

## 文件

- `Bsp_Key.c`：包含按键初始化、按键状态机处理和事件回调函数的实现。
- `Bsp_Key.h`：包含按键驱动的函数声明、状态机定义和回调函数设置。

## 硬件设置

- **按键 1** 连接到 **GPIOA，Pin 0**。
- **按键 2** 连接到 **GPIOC，Pin 13**。

GPIO 引脚配置为无上拉电阻，按键按下时，电平为高。

## 使用方法

### 1. GPIO 初始化

确保在主程序中调用 `KEY_GPIO_Init()` 来初始化按键 GPIO 引脚：

```c
KEY_GPIO_Init();
```

### 2. 按键初始化

使用宏 `User_Key1_Key2_Init` 来初始化两个按键及其 GPIO 端口和引脚，并为单击、双击和长按事件设置默认回调函数：

```c
User_Key1_Key2_Init;
```

`Key_Init()` 函数会初始化按键的状态机并设置回调函数。例如：

```c
Key_Init(&User_Key1, GPIOA, GPIO_PIN_0, Key1_Click_Callback, Key1_Double_Callback, Key1_Long_Callback);
Key_Init(&User_Key2, GPIOC, GPIO_PIN_13, Key2_Click_Callback, Key2_Double_Callback, Key2_Long_Callback);
```

### 3. 按键状态机处理

在主循环中定期调用 `Key_Process()` 来处理每个按键的状态：

```c
Key_Process(&User_Key1);
Key_Process(&User_Key2);
```

### 4. 回调函数

以下是默认的回调函数，用户可以根据需要覆盖这些函数：

- `Key1_Click_Callback()`：按键 1 单击事件。
- `Key1_Double_Callback()`：按键 1 双击事件。
- `Key1_Long_Callback()`：按键 1 长按事件。
- `Key2_Click_Callback()`：按键 2 单击事件。
- `Key2_Double_Callback()`：按键 2 双击事件。
- `Key2_Long_Callback()`：按键 2 长按事件。

默认的回调函数会切换板载 LED 的状态（红色、绿色和蓝色）。

### 5. 状态机流程

按键状态机的工作流程如下：

- **空闲状态（Idle）**：按键未按下。
- **消抖状态（Debounce）**：按键按下，等待消抖处理。
- **等待释放状态（Wait Release）**：按键按下但没有长按，等待松开。
- **等待双击状态（Wait Double）**：检查是否为双击。
- **长按等待释放状态（Long Wait Release）**：长按检测，等待松开。

请参考下面的状态机图示，了解各状态之间的转换：

![示意图](Drawing%202025-01-29%2020.58.04.excalidraw.png = 200)

### 6. 示例代码

```c
#include "Bsp_Key.h"

int main(void) {
    // 初始化按键
    User_Key1_Key2_Init;

    // 主循环
    while (1) {
        // 处理按键事件
        Key_Process(&User_Key1);
        Key_Process(&User_Key2);

        // 其他任务
    }
}
```

## 依赖

- STM32 HAL 库
- 自定义 LED 驱动（参见 `Bsp_LED`）

## 许可证

该项目开源，使用 MIT 许可证。

---
