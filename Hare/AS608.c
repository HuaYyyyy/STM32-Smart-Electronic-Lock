#include <string.h>
#include "main.h"
#include "AS608.h"
#include "usart.h"
uint32_t AS608Addr = 0XFFFFFFFF; //Ĭ��
SysPara AS608Para;    //ָ��ģ��AS608����
uint16_t ValidN;      //ģ������Чģ�����
FP_Task_t g_fp_task = {0}; // ȫ��������ƿ�

extern uint8_t uart1_rx_buf[];          // DMAֱ��д�뻺����
extern uint8_t uart1_rx_pro_buf[];      // ��ѭ����ȫ��ȡ������
extern volatile uint8_t uart1_rx_ready; // ���ڽ��յ����ݱ��


/* ================= �ײ�ͨ���Ż� ================= */
static void Com_SendData(uint8_t data) {
    HAL_UART_Transmit(&huart1, &data, 1, 50);
}
static void SendHead(void) { Com_SendData(0xEF); Com_SendData(0x01); }
static void SendAddr(void) {
    Com_SendData(AS608Addr>>24); Com_SendData(AS608Addr>>16);
    Com_SendData(AS608Addr>>8);  Com_SendData(AS608Addr);
}
static void SendFlag(uint8_t flag) { Com_SendData(flag); }
static void SendLength(int length) { Com_SendData(length>>8); Com_SendData(length); }
static void Sendcmd(uint8_t cmd) { Com_SendData(cmd); }
static void SendCheck(uint16_t check) { Com_SendData(check>>8); Com_SendData(check); }

// �Ż����Ӧ������ (ʹ��ϵͳʱ���)
static uint8_t *JudgeStr(uint16_t waittime) {
    char *data;
    uint8_t str[8];
    str[0]=0xef; str[1]=0x01; str[2]=AS608Addr>>24;
    str[3]=AS608Addr>>16; str[4]=AS608Addr>>8;
    str[5]=AS608Addr; str[6]=0x07; str[7]='\0';
    
    uint32_t start_tick = GET_SYS_TICK();
    // ��������ѯ�����ȴ� waittime ����
    while((GET_SYS_TICK() - start_tick) < waittime) {
        if(uart1_rx_ready) {
            data = strstr((const char*)&uart1_rx_pro_buf, (const char*)str);
            uart1_rx_ready = 0;
            if(data) return (uint8_t*)data;
        }
    }
    return 0;
}

/* ================= ģ��ָ�  ================= */
static uint8_t GZ_GetImage(void) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x03); Sendcmd(0x01);
    temp = 0x01+0x03+0x01; SendCheck(temp);
    data = JudgeStr(500); // ģ����Ӧͨ����100ms��
    if(data) ensure = data[9]; else ensure = 0xff;
    return ensure;
}

static uint8_t GZ_GenChar(uint8_t BufferID) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x04); Sendcmd(0x02);
    Com_SendData(BufferID);
    temp = 0x01+0x04+0x02+BufferID; SendCheck(temp);
    data = JudgeStr(500);
    if(data) ensure = data[9]; else ensure = 0xff;
    return ensure;
}

static uint8_t GZ_Match(void) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x03); Sendcmd(0x03);
    temp = 0x01+0x03+0x03; SendCheck(temp);
    data = JudgeStr(500);
    if(data) ensure = data[9]; else ensure = 0xff;
    return ensure;
}

static uint8_t GZ_RegModel(void) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x03); Sendcmd(0x05);
    temp = 0x01+0x03+0x05; SendCheck(temp);
    data = JudgeStr(500);
    if(data) ensure = data[9]; else ensure = 0xff;
    return ensure;
}

uint8_t GZ_StoreChar(uint8_t BufferID, uint16_t PageID) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x06); Sendcmd(0x06);
    Com_SendData(BufferID); Com_SendData(PageID>>8); Com_SendData(PageID);
    temp = 0x01+0x06+0x06+BufferID+(PageID>>8)+(uint8_t)PageID; SendCheck(temp);
    data = JudgeStr(500);
    if(data) ensure = data[9]; else ensure = 0xff;
    return ensure;
}

uint8_t GZ_Empty(void) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x03); Sendcmd(0x0D);
    temp = 0x01+0x03+0x0D; SendCheck(temp);
    data = JudgeStr(1000);
    if(data) ensure = data[9]; else ensure = 0xff;
    return ensure;
}

uint8_t GZ_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x08); Sendcmd(0x1b);
    Com_SendData(BufferID); Com_SendData(StartPage>>8); Com_SendData(StartPage);
    Com_SendData(PageNum>>8); Com_SendData(PageNum);
    temp = 0x01+0x08+0x1b+BufferID+(StartPage>>8)+(uint8_t)StartPage+(PageNum>>8)+(uint8_t)PageNum;
    SendCheck(temp);
    data = JudgeStr(800);
    if(data) {
        ensure = data[9];
        p->pageID = (data[10]<<8)+data[11];
        p->mathscore = (data[12]<<8)+data[13];
    } else ensure = 0xff;
    return ensure;
}

uint8_t GZ_ValidTempleteNum(uint16_t *ValidN) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x03); Sendcmd(0x1d);
    temp = 0x01+0x03+0x1d; SendCheck(temp);
    data = JudgeStr(500);
    if(data) { ensure = data[9]; *ValidN = (data[10]<<8)+data[11]; }
    else ensure = 0xff;
    return ensure;
}

uint8_t GZ_ReadSysPara(SysPara *p) {
    uint16_t temp; uint8_t ensure; uint8_t *data;
    SendHead(); SendAddr(); SendFlag(0x01); SendLength(0x03); Sendcmd(0x0F);
    temp = 0x01+0x03+0x0F; SendCheck(temp);
    data = JudgeStr(500);
    if(data) {
        ensure = data[9]; p->GZ_max = (data[14]<<8)+data[15]; p->GZ_level = data[17];
        p->GZ_addr = (data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
        p->GZ_size = data[23]; p->GZ_N = data[25];
    } else ensure = 0xff;
    return ensure;
}

uint8_t GZ_HandShake(uint32_t *GZ_Addr) {
    SendHead(); SendAddr(); Com_SendData(0X01); Com_SendData(0X00); Com_SendData(0X00);
    uint32_t start = GET_SYS_TICK();
    while(GET_SYS_TICK() - start < 200) { // �滻 HAL_Delay(200)
        if(uart1_rx_ready) {
            if(uart1_rx_pro_buf[0]==0XEF && uart1_rx_pro_buf[1]==0X01 && uart1_rx_pro_buf[6]==0X07) {
                *GZ_Addr = (uart1_rx_pro_buf[2]<<24)+(uart1_rx_pro_buf[3]<<16)+(uart1_rx_pro_buf[4]<<8)+(uart1_rx_pro_buf[5]);
                uart1_rx_ready = 0; return 0;
            }
            uart1_rx_ready = 0;
        }
    }
    return 1;
}

/* ================= ��ʼ�� ================= */
FP_StatusTypeDef FR_Init(void) {
    if(HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rx_buf, RX_BUF_SIZE) != HAL_OK) Error_Handler();
    uint32_t start = GET_SYS_TICK();
    while(GZ_HandShake(&AS608Addr)) {
        if(GET_SYS_TICK() - start > 3000) return FP_TIMEOUT; // 3�����ֳ�ʱ
    }
    GZ_ValidTempleteNum(&ValidN);
    GZ_ReadSysPara(&AS608Para);
    return FP_OK;
}

/* ================= ���ģ�����������״̬�� ================= */
void FP_Task_Reset(void) {
    g_fp_task.step = 0;
}

// ������¼��ָ�� (��ѭ�����ڵ���)
FP_NonBlockStatus_t FP_Task_Add(uint16_t target_id) {
    uint8_t ensure;
    g_fp_task.target_id = target_id;

    switch(g_fp_task.step) {
        case 0: // ��һ�β�ͼ
            ensure = GZ_GetImage();
            if(ensure == 0x00) {
                ensure = GZ_GenChar(CharBuffer1);
                if(ensure == 0x00) { g_fp_task.step = 1; return FP_PROCESSING; }
            } else if(ensure == 0x02) return FP_WAITING; // û��ָ�������ó�CPU��
            return FP_ERROR_NB;
            
        case 1: // �ڶ��β�ͼ
            ensure = GZ_GetImage();
            if(ensure == 0x00) {
                ensure = GZ_GenChar(CharBuffer2);
                if(ensure == 0x00) { g_fp_task.step = 2; return FP_PROCESSING; }
            } else if(ensure == 0x02) return FP_WAITING; 
            g_fp_task.step = 0; return FP_ERROR_NB;
            
        case 2: // �ȶ�
            ensure = GZ_Match();
            if(ensure == 0x00) { g_fp_task.step = 3; return FP_PROCESSING; }
            g_fp_task.step = 0; return FP_MISMATCH_NB;
            
        case 3: // �ϲ�
            ensure = GZ_RegModel();
            if(ensure == 0x00) { g_fp_task.step = 4; return FP_PROCESSING; }
            g_fp_task.step = 0; return FP_ERROR_NB;
            
        case 4: // �洢
            ensure = GZ_StoreChar(CharBuffer2, g_fp_task.target_id);
            g_fp_task.step = 0; 
            if(ensure == 0x00) { GZ_ValidTempleteNum(&ValidN); return FP_OK_NB; }
            return FP_ERROR_NB;
    }
    return FP_ERROR_NB;
}

// ����������ָ�� (��ѭ�����ڵ���)
FP_NonBlockStatus_t FP_Task_Search(void) {
    uint8_t ensure;
    switch(g_fp_task.step) {
        case 0: // ��ͼ
            ensure = GZ_GetImage();
            if(ensure == 0x00) {
                ensure = GZ_GenChar(CharBuffer1);
                if(ensure == 0x00) { g_fp_task.step = 1; return FP_PROCESSING; }
            } else if(ensure == 0x02) return FP_WAITING; // û��ָ���ó�CPU
            return FP_ERROR_NB;
            
        case 1: // ����
            ensure = GZ_HighSpeedSearch(CharBuffer1, 0, 300, &g_fp_task.search_res);
            g_fp_task.step = 0; 
            if(ensure == 0x00) {
                if(g_fp_task.search_res.mathscore > 100) return FP_MATCH_NB;
                else return FP_MISMATCH_NB;
            }
            return FP_MISMATCH_NB;
    }
    return FP_ERROR_NB;
}

// ����ԭ�е�����API�Է������ط����ã�������������ѭ��ʹ�ã�
FP_StatusTypeDef Add_FR(void) { return FP_TIMEOUT; } 
FP_StatusTypeDef Press_FR(void) { return FP_TIMEOUT; }

//ģ��Ӧ���ȷ������Ϣ����
//���ܣ�����ȷ���������Ϣ������Ϣ
//����: ensure
//const char *EnsureMessage(uint8_t ensure) 
//{
//	const char *p;
//	switch(ensure)
//	{
//		case  0x00:
//			p="OK";break;		
//		case  0x01:
//			p="���ݰ����մ���";break;
//		case  0x02:
//			p="��������û����ָ";break;
//		case  0x03:
//			p="¼��ָ��ͼ��ʧ��";break;
//		case  0x04:
//			p="ָ��ͼ��̫�ɡ�̫��������������";break;
//		case  0x05:
//			p="ָ��ͼ��̫ʪ��̫��������������";break;
//		case  0x06:
//			p="ָ��ͼ��̫�Ҷ�����������";break;
//		case  0x07:
//			p="ָ��ͼ����������������̫�٣������̫С��������������";break;
//		case  0x08:
//			p="ָ�Ʋ�ƥ��";break;
//		case  0x09:
//			p="û������ָ��";break;
//		case  0x0a:
//			p="�����ϲ�ʧ��";break;
//		case  0x0b:
//			p="����ָ�ƿ�ʱ��ַ��ų���ָ�ƿⷶΧ";
//		case  0x10:
//			p="ɾ��ģ��ʧ��";break;
//		case  0x11:
//			p="���ָ�ƿ�ʧ��";break;	
//		case  0x15:
//			p="��������û����Чԭʼͼ��������ͼ��";break;
//		case  0x18:
//			p="��д FLASH ����";break;
//		case  0x19:
//			p="δ�������";break;
//		case  0x1a:
//			p="��Ч�Ĵ�����";break;
//		case  0x1b:
//			p="�Ĵ����趨���ݴ���";break;
//		case  0x1c:
//			p="���±�ҳ��ָ������";break;
//		case  0x1f:
//			p="ָ�ƿ���";break;
//		case  0x20:
//			p="��ַ����";break;
//		default :
//			p="ģ�鷵��ȷ��������";break;
//	}
// return p;	
//}






