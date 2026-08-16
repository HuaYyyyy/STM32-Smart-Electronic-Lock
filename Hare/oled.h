
#ifndef __OLED_H
#define __OLED_H			  	 
#include "main.h"  
#include "stm32f1xx_hal_gpio.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  

//-----------------OLED IIC�˿ڶ���---------------- PB0,PB1  					   
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)//SCL IIC�ӿڵ�ʱ���ź�
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)//SDA IIC�ӿڵ������ź�
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)
 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//OLED�����ú���
void OLED_Init(void);
void OLED_Clear(void);

void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    

void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p);
void OLED_ClearLine(u8 y);     // 清除指定行 (填充空格)
void OLED_ShowString_Small(u8 x,u8 y, u8 *p);  // 6x8小字体显示字符串
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void fill_picture(unsigned char fill_Data);
void OLED_ShowCHinese1(u8 x,u8 y,u8 no);  //反显


void OLED_WR_Byte(unsigned dat,unsigned cmd);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);

//void OLED_DrawPoint(u8 x,u8 y,u8 t);
//void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);

#endif  
