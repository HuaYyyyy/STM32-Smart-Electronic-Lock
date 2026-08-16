//--------------------------------------------------------------------   //
//    使用软件定时器，务必在stm32f1xx_it.c文件中                          //
//    1、在文件开头中声明：extern volatile uint32_t g_sys_tick;           //
//    2、找到void SysTick_Handler(void) 函数，增加一行：g_sys_tick++;     //
//---------------------------------------------------------------------- //
#include "softtimer.h"

/* 1. 定义全局时间戳变量，必须加 volatile 防止编译器优化 */
volatile uint32_t g_sys_tick = 0;

//
///* 2. SysTick 中断服务函数 (每 1ms 进一次) */
//void SysTick_Handler(void) {
//    g_sys_tick++; 
//    // 如果使用 HAL 库，这里通常调用 HAL_IncTick();
//}

///* 3. 获取当前系统时间的宏 */
//#define GET_SYS_TICK() (g_sys_tick)

// 启动定时器
void Timer_Start(SoftTimer_t* timer, uint32_t ms) {
    timer->start_time = GET_SYS_TICK();
    timer->duration = ms;
    timer->is_running = 1;
}

// 停止定时器
void Timer_Stop(SoftTimer_t* timer) {
    timer->is_running = 0;
}

// 检查定时器是否超时 (核心逻辑)
uint8_t Timer_IsExpired(SoftTimer_t* timer) {
    if (!timer->is_running) return 0; // 没启动，直接返回 false
    
    if (IS_TIMEOUT(timer->start_time, timer->duration)) {
        timer->is_running = 0; // 超时后自动停止，防止重复触发
        return 1;              // 返回 true，表示时间到了
    }
    return 0; // 还没到时间
}
