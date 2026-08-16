#ifndef __FACEID_MAIN_H
#define __FACEID_MAIN_H

#include "main.h"

#define   UART_BUFF_SIZE  50  /////

/***********************ȫ�ֱ���****************************/
typedef struct __UsartReceiveData{
	
	uint8_t  Uart_RxBuff[UART_BUFF_SIZE];
	uint8_t  Uart_TxBuff[UART_BUFF_SIZE];
	uint16_t Rx_Len;
}UsartData;

extern  UsartData Usart3Data;
extern	uint8_t Data_Send_Cmd;
extern	uint8_t	Display_Cmd;
extern	uint16_t Face_ID;

/*-------------------------------------------------*/
/*                   ��������                      */
/*-------------------------------------------------*/
void FaceID_Init(void);
void FaceID_Main_Loop(void);

//void MyUart1_Ctr_Handler(void);
//void Uart1_Send_OK(void);
//void Uart1_Send_ADD_User_ID(void);

void Uart_Send_Delete_Point_User_ID(uint8_t H_D,uint8_t L_D);
void Uart_Delete_all_User_ID(void);
void Uart_Send_Get_All_User_ID(void);
void Uart_Send_Delete_User_ID(uint16_t  ID);
void Uart_Send_Verify(void);
void Uart_Send_Reset(void);
uint8_t Check_Sum_Xor(uint8_t *Comm_Buff,uint16_t Len);
void UART3_Send(uint16_t len);
//void Uart1_Send_EnrollSingleWithID_Point_User_ID(uint8_t H_ID,uint8_t L_ID);
void Uart3_Send_EnrollSingleWithID_Point_User_ID(void); //����ƣ����������������
#endif
