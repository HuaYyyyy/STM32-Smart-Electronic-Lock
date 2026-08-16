#include "faceid_key_dis.h"
#include "gpio.h"

/*-------------------------------------------------*/
/*                  ȫ�ֱ���                       */
/*-------------------------------------------------*/
#define FACE_DIS_LINE_NUM 6     //����ʶ�����ʾ���������һ��
static void OLED_Clear_Line(u8 nline);

/* KEY_SCAN 已移至 AppController 状态机 (freertos.c) */
void  KEY_SCAN(uint8_t key) // (deprecated)�ⲿ����ɨ����򣬲�����
{
  char ch = Keypad_ToChar(key);
  switch(ch)
  {
    case '1'://¼������									
      Display_Cmd	 = 0x06; //¼������									
			Data_Send_Cmd = 0x02; //������������ ¼����������
      break;
    case '2'://�������
			Display_Cmd = 0x0A;
      Data_Send_Cmd = 0x03; //�������      
      break;
    case 'C'://ʶ������
      Display_Cmd	 = 0x01; //��ʼʶ��
      Data_Send_Cmd = 0x01;//��ʼ����ʶ��
      break;
    case '0':
      break;
    default:
      break;
  }			
}

void  OLED_DISPLAY(void)  //ѭ����ʾ������������
{
	if(Display_Cmd != 0x00)
	{
		switch(Display_Cmd)
		{
			case 0x01: //��ʾ��ʼʶ��
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
        OLED_ShowCHinese(0,FACE_DIS_LINE_NUM,5);//��ʾ����
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,6);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,7);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,8);//��ʾ����
				Display_Cmd= RESET;
				break;
			case 0x02:      /*ʶ��ɹ�*/
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
        OLED_ShowCHinese(0,FACE_DIS_LINE_NUM,7);//��ʾ����
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,8);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,9);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,10);//��ʾ����
        OLED_ShowString(72,FACE_DIS_LINE_NUM,(u8 *)"ID");//��ʾ�ַ�
        OLED_ShowNum(90,FACE_DIS_LINE_NUM,Face_ID,2,16);//  
				osDelay(2000);
				Display_Cmd= 0x05;
				break;
			case 0x03: //ʶ��ʧ��
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,7);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,8);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,11);//��ʾ����
				OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,12);//��ʾ����
				osDelay(2000);
				Display_Cmd= 0x05;
				break;
			case 0x04:  /***δע��*/
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,13);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,14);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,15);//��ʾ����
				osDelay(2000);
				Display_Cmd= 0x05;
				break;
			case 0x05:  //ʶ�������
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,7);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,8);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,16);//��ʾ����
				OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,17);//��ʾ����
				OLED_ShowCHinese(90,FACE_DIS_LINE_NUM,18);//��ʾ����
				Display_Cmd= RESET;
				break;
//			case 0x06:  //¼��������
//				//OLED_Clear();//OLED����
//        OLED_Clear_Line(FACE_DIS_LINE_NUM);
//				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,19);//��ʾ����
//				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,20);//��ʾ����
//				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,21);//��ʾ����
//				OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,22);//��ʾ����
//				OLED_ShowCHinese(90,FACE_DIS_LINE_NUM,18);//��ʾ����
//				Display_Cmd= RESET;
//				break;
			case 0x06:  //¼��������
    OLED_Clear_Line(FACE_DIS_LINE_NUM);
    OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,19);
    OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,20);
    OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,21);
    OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,22);
    OLED_ShowCHinese(90,FACE_DIS_LINE_NUM,18);
    osDelay(100);   // 缩短延迟, 让FaceID_Main_Loop能及时处理响应
    Display_Cmd= 0x05;
    break;
			case 0x07:  //¼��ɹ�
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
        OLED_ShowCHinese(0,FACE_DIS_LINE_NUM,19);//��ʾ����
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,20);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,9);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,10);//��ʾ����
        OLED_ShowString(72,FACE_DIS_LINE_NUM,(u8 *)"ID");//��ʾ�ַ�
        OLED_ShowNum(90,FACE_DIS_LINE_NUM,Face_ID,2,16);// 
			  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
				osDelay(2000);
			  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
				Display_Cmd= 0x05;
				break;
			case 0x08:
			{//�����Ѵ���
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,21);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,22);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,23);//��ʾ����
				OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,24);//��ʾ����
				OLED_ShowCHinese(90,FACE_DIS_LINE_NUM,25);//��ʾ����
				osDelay(2000);
				Display_Cmd= 0x05;
				break;
			}
			case 0x09:  //¼��ʧ��
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,19);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,20);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,11);//��ʾ����
				OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,12);//��ʾ����
				osDelay(2000);
				Display_Cmd= 0x05;
				break;
			case 0x0A:	//���������
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,26);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,27);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,21);//��ʾ����
				OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,22);//��ʾ����
				OLED_ShowCHinese(90,FACE_DIS_LINE_NUM,18);//��ʾ����
				osDelay(2000);
				Display_Cmd= 0X0B;
				break;
			case 0x0B:  //����ɹ�
				//OLED_Clear();//OLED����
        OLED_Clear_Line(FACE_DIS_LINE_NUM);
				OLED_ShowCHinese(18,FACE_DIS_LINE_NUM,26);//��ʾ����
				OLED_ShowCHinese(36,FACE_DIS_LINE_NUM,27);//��ʾ����
				OLED_ShowCHinese(54,FACE_DIS_LINE_NUM,9);//��ʾ����
				OLED_ShowCHinese(72,FACE_DIS_LINE_NUM,10);//��ʾ����			
			  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
				osDelay(4000);
			  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
				Display_Cmd= 0x05;
				break;
			case 0x0C://����
				OLED_Clear();//OLED����				
				Display_Cmd= RESET;
				break;			
			default:
				break;
		}
  }
}

static void OLED_Clear_Line(u8 nline)
{
  u8 i; 
  for(i=0; i<16; i++)
  {
    OLED_ShowChar(i*8,nline,' '); 
  }
}
	
