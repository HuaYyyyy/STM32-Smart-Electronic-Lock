#ifndef __SOFTTIMER_H
#define __SOFTTIMER_H			  	 

#include "main.h" 
extern volatile uint32_t g_sys_tick;

/* 3. ��ȡ��ǰϵͳʱ��ĺ� */
#define GET_SYS_TICK() (HAL_GetTick())  // 用HAL硬件定时器, 不依赖RTOS tick

/* �ж��Ƿ�ʱ�ĺ� 
 * ԭ������ǰʱ�� - ��ʼʱ�� >= Ŀ����ʱ 
 * ע�⣺ʹ�� uint32_t �������������� g_sys_tick ���(Լ49.7��)������
 */

#define IS_TIMEOUT(start_time, duration_ms) \
    ((GET_SYS_TICK() - (start_time)) >= (duration_ms))

typedef struct {
    uint32_t start_time;  // ��¼����ʱ��ʱ���
    uint32_t duration;    // ��Ҫ��ʱ��ʱ�� (ms)
    uint8_t  is_running;  // ��ʱ���Ƿ���������
} SoftTimer_t;

void Timer_Start(SoftTimer_t* timer, uint32_t ms);
void Timer_Stop(SoftTimer_t* timer);
uint8_t Timer_IsExpired(SoftTimer_t* timer);

#endif
