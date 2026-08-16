#include "keypad.h"

/* 按键编码映射表 */
static const uint8_t key_map[4][4] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {8, 9, 10, 11},
    {12, 13, 14, 15}
};

volatile uint8_t g_key_val = KEY_NONE;

/* 内部状态变量（中断中维护） */
static uint8_t last_raw_key = KEY_NONE;
static uint8_t debounce_cnt = 0;
static uint8_t stable_key = KEY_NONE;
static uint8_t reported_key = KEY_NONE;
#define DEBOUNCE_THR 3  // 消抖阈值：3次中断 = 30ms

/* 快速原始扫描（无延时，适合中断调用） */
static uint8_t raw_scan(void) {
    uint16_t row_pins[4] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
    uint16_t col_pins[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};

    // 所有行线拉高
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);

    for (uint8_t r = 0; r < 4; r++) {
        // 拉低当前行
        HAL_GPIO_WritePin(GPIOA, row_pins[r], GPIO_PIN_RESET);
        // 电平稳定需要极短时间，插入 3 个 NOP 即可（约 50ns @ 72MHz）
        __NOP(); __NOP(); __NOP();

        for (uint8_t c = 0; c < 4; c++) {
            if (HAL_GPIO_ReadPin(GPIOA, col_pins[c]) == GPIO_PIN_RESET) {
                // 恢复所有行线高电平，避免扫描间隙产生误触电流
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
                return key_map[r][c];
            }
        }
        // 恢复当前行线高电平
        HAL_GPIO_WritePin(GPIOA, row_pins[r], GPIO_PIN_SET);
    }
    return KEY_NONE;
}

void Keypad_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    extern TIM_HandleTypeDef htim3;  // 使用 tim.c 中的全局 htim3

    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 列线 PA0~PA3：输入 + 内部上拉
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 行线 PA4~PA7：推挽输出
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);

    // 重新配置全局 htim3 为 10ms 周期（72MHz / 720 / 1000 = 100Hz = 10ms）
    // htim3 已在 MX_TIM3_Init() 中由 HAL_TIM_Base_Init() 初始化
    __HAL_TIM_SET_AUTORELOAD(&htim3, 1000 - 1);
    __HAL_TIM_SET_PRESCALER(&htim3, 720 - 1);
    __HAL_TIM_SET_COUNTER(&htim3, 0);

    // 启动定时器并开启中断
    HAL_TIM_Base_Start_IT(&htim3);
}

/* 由 HAL_TIM_PeriodElapsedCallback (main.c) 调用的按键扫描ISR */
void Keypad_ScanISR(void) {
    uint8_t current_raw = raw_scan();

    // 1. 消抖逻辑：连续 N 次读取相同值才认为稳定
    if (current_raw == last_raw_key) {
        if (debounce_cnt < DEBOUNCE_THR) debounce_cnt++;
    } else {
        debounce_cnt = 0;
        last_raw_key = current_raw;
    }

    // 更新稳定状态
    if (debounce_cnt >= DEBOUNCE_THR) {
        stable_key = current_raw;
    } else {
        stable_key = KEY_NONE;
    }

    // 2. 按键事件判定（按下检测 & 释放重置）
    if (stable_key != KEY_NONE && reported_key == KEY_NONE) {
        g_key_val = stable_key;      // 上报新按键
        reported_key = stable_key;   // 标记已上报
    } else if (stable_key == KEY_NONE && reported_key != KEY_NONE) {
        reported_key = KEY_NONE;     // 按键已释放，准备下一次检测
    }
}

// 字符映射（供主循环调用）
char Keypad_ToChar(uint8_t key_code) {
    static const char key_chars[16] = {'D','#','0','*','C','9','8','7','B','6','5','4','A','3','2','1'};
    return (key_code < 16) ? key_chars[key_code] : '\0';
}

