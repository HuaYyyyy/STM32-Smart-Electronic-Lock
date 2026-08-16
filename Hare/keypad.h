#ifndef __KEYPAD_H
#define __KEYPAD_H

#include "main.h"

#define KEY_NONE 0xFF

// 供主循环读取的按键值（volatile 保证中断与主循环可见性）
extern volatile uint8_t g_key_val;

void Keypad_Init(void);
char Keypad_ToChar(uint8_t key_code);

/* 由 HAL_TIM_PeriodElapsedCallback 调用的按键扫描ISR */
void Keypad_ScanISR(void);

// 定时器中断服务函数（需放在 stm32f1xx_it.c 或对应 IT 文件中）
//void TIM3_IRQHandler(void);

#endif

