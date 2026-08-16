#include "usart.h"
#include <string.h>
#include "faceid_main.h"
#include "gpio.h"

/*-------------------------------------------------*/
/*                  全局变量                       */
/*-------------------------------------------------*/
UsartData Usart3Data;
uint8_t Data_Send_Cmd = 0;
uint8_t	Display_Cmd = 0;
uint16_t Face_ID;

extern uint8_t uart3_rx_buf[];
extern volatile uint8_t uart3_rx_ready;/* 接收状态标志 */

/*-------------------------------------------------*/
/*                 本任务变量                      */
/*-------------------------------------------------*/

/*-------------------------------------------------*/
/*函数名：配置函数                                 */
/*-------------------------------------------------*/

/***************************************************************

***************************************************************/
static void UART3_Send(uint16_t len)// use uart3
{

	memset(&Usart3Data.Uart_TxBuff[len],0x00,UART_BUFF_SIZE-len);  //把多余的数据清理点////
	HAL_UART_Transmit_DMA(&huart3,Usart3Data.Uart_TxBuff,len); 
}

/**************************函数定义结束*************************/
/***************************************************************

***************************************************************/
static uint8_t Check_Sum_Xor(uint8_t *Comm_Buff,uint16_t Len) //异或校验。
{
	uint16_t i = 0;
	uint8_t  Check_Sum = 0;
	for(i = 0; i < Len; i++)
	{
		Check_Sum = *Comm_Buff^Check_Sum;
		Comm_Buff++;
	}
	return Check_Sum; //
}
/**复位命令。。。*/
void Uart_Send_Reset(void)      
{
	uint8_t i;
	i = 0;
	Usart3Data.Uart_TxBuff[i++] = 0xEF;   //固定的消息开头同步字 0xEF 0xAA
	Usart3Data.Uart_TxBuff[i++] = 0xAA;		//固定的消息开头同步字 0xEF 0xAA
	Usart3Data.Uart_TxBuff[i++] = 0x10;		//消息 ID   0X10复位信息
	Usart3Data.Uart_TxBuff[i++] = 0x00;		//数据大小 几个字节。高8位 数据位 高字节
	Usart3Data.Uart_TxBuff[i++] = 0x00;		//数据大小 几个字节。低8位   后边没数据位了  数据位低字节。
	Usart3Data.Uart_TxBuff[i++] = 0x10;		//没有数据位了 这位就是校验位
	UART3_Send(i);
}

/**核实发送识别。**/
void Uart_Send_Verify(void)   //串口3给 屏幕识别的信息 。  解锁 开始 识别人脸 开始比对。  开始识别。。。。
{
	uint8_t i;
	i = 0;
	Usart3Data.Uart_TxBuff[i++] = 0xEF;//固定的消息开头同步字 0xEF 0xAA
	Usart3Data.Uart_TxBuff[i++] = 0xAA;//固定的消息开头同步字 0xEF 0xAA
	Usart3Data.Uart_TxBuff[i++] = 0x12;//消息 ID   0X12复位信息, 解锁。 解锁后断电。？？？？？
	Usart3Data.Uart_TxBuff[i++] = 0x00; //数据大小 高8位
	Usart3Data.Uart_TxBuff[i++] = 0x02;//数据大小 低8位
	Usart3Data.Uart_TxBuff[i++] = 0x00; //数据
	Usart3Data.Uart_TxBuff[i++] = 0x0A;	//数据 超时时间
	Usart3Data.Uart_TxBuff[i++] = 0x1A;  //异或校验   异或校验是 从 EF AA 的后边 开始的  最后一位是异或校验
	UART3_Send(i);

}
/*删除人员的ID*/ //这个是删除指定人员的ID
void Uart_Send_Delete_User_ID(uint16_t  ID)
{
	uint8_t i;
	i = 0;
	Usart3Data.Uart_TxBuff[i++] = 0xEF;
	Usart3Data.Uart_TxBuff[i++] = 0xAA;
	Usart3Data.Uart_TxBuff[i++] = 0x20;  //删除用户   //删除指定的用户。
	Usart3Data.Uart_TxBuff[i++] = 0x00;		//数据长度高字节
	Usart3Data.Uart_TxBuff[i++] = 0x02;		//数据长度低字节
	Usart3Data.Uart_TxBuff[i++] = (ID >> 8) &0XFF;  //取高8位
	Usart3Data.Uart_TxBuff[i++] = ID & 0XFF;				//取低8位
	Usart3Data.Uart_TxBuff[i++] = Check_Sum_Xor(&Usart3Data.Uart_TxBuff[2],5);
	
	UART3_Send(i);

}

/**读取模组内所有的 ID的命令协议不同单独处理。**/
void Uart_Send_Get_All_User_ID(void) //读取模组内部所有用户数量和用户ID列表  
{
	uint8_t i;
	i = 0;
	Usart3Data.Uart_TxBuff[i++] = 0xEF;
	Usart3Data.Uart_TxBuff[i++] = 0xAA;
	Usart3Data.Uart_TxBuff[i++] = 0x24; //读取模组内部所有用户数量和用户ID列表，
	Usart3Data.Uart_TxBuff[i++] = 0x00; //高字节
	Usart3Data.Uart_TxBuff[i++] = 0x01; //低字节 
	Usart3Data.Uart_TxBuff[i++] = 0x00; //低字节  
	Usart3Data.Uart_TxBuff[i++] = 0x25;  //异或校验。。。。。。
	UART3_Send(i);
}


//**删除所有命令。
void	Uart_Delete_all_User_ID(void)
{
	uint8_t i;
	i = 0;
	Usart3Data.Uart_TxBuff[i++] = 0xEF;
	Usart3Data.Uart_TxBuff[i++] = 0xAA;
	Usart3Data.Uart_TxBuff[i++] = 0x21; //读取模组内部所有用户数量和用户ID列表，
	Usart3Data.Uart_TxBuff[i++] = 0x00; //高字节
	Usart3Data.Uart_TxBuff[i++] = 0x00; //低字节  
	Usart3Data.Uart_TxBuff[i++] = 0x21;  //异或校验。。。。。。
	UART3_Send(i);
	
}

///***删除指定人员的ID 
void Uart_Send_Delete_Point_User_ID(uint8_t H_D,uint8_t L_D)
{
	uint8_t i;
	i = 0;
	Usart3Data.Uart_TxBuff[i++] = 0xEF;
	Usart3Data.Uart_TxBuff[i++] = 0xAA;
	Usart3Data.Uart_TxBuff[i++] = 0x20;  //删除用户   //删除指定的用户。
	Usart3Data.Uart_TxBuff[i++] = 0x00;		//数据长度高字节
	Usart3Data.Uart_TxBuff[i++] = 0x02;		//数据长度低字节
	Usart3Data.Uart_TxBuff[i++] = H_D;//取高8位
	Usart3Data.Uart_TxBuff[i++] =	L_D;//取低8位
	Usart3Data.Uart_TxBuff[i++] = Check_Sum_Xor(&Usart3Data.Uart_TxBuff[2],5);
	
	UART3_Send(i);
}

/**手动单帧 添加人脸命令**/
void Uart3_Send_EnrollSingleWithID_Point_User_ID(void)
{
	uint8_t i;
	i = 0;
	Usart3Data.Uart_TxBuff[i++] = 0xEF;
	Usart3Data.Uart_TxBuff[i++] = 0xAA;
	Usart3Data.Uart_TxBuff[i++] = 0x1D;  //录入用户。
	Usart3Data.Uart_TxBuff[i++] = 0x00;		//数据长度高字节
	Usart3Data.Uart_TxBuff[i++] = 0x23;		//数据长度低字节
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] = 0x00;
	Usart3Data.Uart_TxBuff[i++] =	0x0A;
	Usart3Data.Uart_TxBuff[i++] = Check_Sum_Xor(&Usart3Data.Uart_TxBuff[2],Usart3Data.Uart_TxBuff[4]+3);
	
	UART3_Send(i);
	
}

void FaceID_Main_Loop(void)  //主循环函数。
{
	/*发送命令判断*/
	if(Data_Send_Cmd != 0x00)
	{
		switch(Data_Send_Cmd) //发送命令
		{
				case 0x01: //启动人脸识别 
				{//
					Uart_Send_Verify();
					Data_Send_Cmd = RESET; //单次循环命令清零。
					break;
				}
				case 0x02:    //启动添加人脸命令。。。
				{
					Uart3_Send_EnrollSingleWithID_Point_User_ID();
					Data_Send_Cmd = RESET; //单次循环命令清零。
					break;
				}
				case 0x03:    //删除所有人脸特征的命令。。。。
				{
					Uart_Delete_all_User_ID();
					Data_Send_Cmd = RESET; //单次循环命令清零。
					break;
				}
				case 0x04:    //模组复位命令 备用未使用。。
				{
					Uart_Send_Reset();
					Data_Send_Cmd = RESET; //单次循环命令清零。
					break;
				}
				case 0x06:    //。。。
				{
					Data_Send_Cmd = RESET; //单次循环命令清零。
					break;
				}
				default:
				{
					break;
					
				}
		
		}
	}
  
	// 数据接收完成
  if(uart3_rx_ready)//接收完成标志位。在中断里设置的接收完备
	{
    //接收判断是什么状态。
		switch(Usart3Data.Uart_RxBuff[5]) //收到模块发送了过来的数据
		{
			case 0x10:    //这个是异或校验值,复位成功 /****复位命令  在自己单独的工程项目中需要时候使用****/
				Usart3Data.Uart_RxBuff[5] = 0;
				break;
			case 0x01://通知包,备用未使用    
				Usart3Data.Uart_RxBuff[5] = 0;				
				break;			
			case 0x12: //识别
				if(Usart3Data.Uart_RxBuff[6] == 0x00 &&((Usart3Data.Uart_RxBuff[7] << 8) + Usart3Data.Uart_RxBuff[8]) < 101) //ID最大的值是1----100   //识别成功后有数据。   
				{/*识别成功后的ID*/
						Face_ID =	Usart3Data.Uart_RxBuff[7]<<8 |Usart3Data.Uart_RxBuff[8];						
						Display_Cmd = 0x02;//识别成功,显示人脸识别成功。
				}
				else if(Usart3Data.Uart_RxBuff[6] == 0x08 && Usart3Data.Uart_RxBuff[7] == 0x18) //没有已录入的用户  未检测到人脸  
				{
						Display_Cmd = 0x04;//显示未注册
				}			//如果都不是识别到的数据。都不是识别到的ID		
				else  //   EF AA 00 00 26 12 08 FF FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3C  
				{     //对应的识别失败。Uart_RxBuff[7]  Uart_RxBuff[8]  是 FF FF 就是识别失败 成功会返回识别到的正确ID。
						Display_Cmd = 0x03;//显示识别失败						
				}
				break;   
			case 0x1D:	 //这个是 添加人脸特征值返回的数据。
				if(Usart3Data.Uart_RxBuff[6] == 0x00)//注册成功
        {
          Face_ID =	Usart3Data.Uart_RxBuff[7]<<8 |Usart3Data.Uart_RxBuff[8];//添加成功后返回添加的ID值是多少。          
          Display_Cmd = 0x07;//去显示录入成功        
        }			
				else if(Usart3Data.Uart_RxBuff[6]==0x0A) //杭州模组不适用。
        {
          Display_Cmd = 0x08;//显示人脸已存在
        }
				else if(Usart3Data.Uart_RxBuff[6]==0x0D) //杭州模组录入超时  录入的时候没有人脸的情况下。
        {
          Display_Cmd = 0x09;//显示录入失败        
        }
				else
        {
          Display_Cmd = 0x09;//显示录入失败
        }			
				break;	
			case 0x21:  // 删除所有人脸 回复命令  
				Display_Cmd = 0x0A; //显示清除人脸中 然后显示清除成功。
				break;
			default:
				break;
		}
		uart3_rx_ready = 0;//重新复位				
	}	
}

void FaceID_Init(void){
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, uart3_rx_buf, RX_BUF_SIZE);
  Uart_Send_Reset();
}
	
