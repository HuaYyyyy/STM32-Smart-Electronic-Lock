#ifndef __AS608_H
#define __AS608_H

#include "main.h"
#include "softtimer.h" // 引入系统时间戳

#define CharBuffer1 0x01
#define CharBuffer2 0x02

typedef struct  
{
	uint16_t pageID;//指纹ID
	uint16_t mathscore;//匹配得分
}SearchResult;

typedef struct
{
	uint16_t GZ_max;//指纹最大容量
	uint8_t  GZ_level;//安全等级
	uint32_t GZ_addr;
	uint8_t  GZ_size;//通讯数据包大小
	uint8_t  GZ_N;//波特率基数N
}SysPara;

typedef enum
{
  FP_OK       = 0x00U,  //正常执行
  FP_MATCH    = 0x01U,  //指纹匹配
  FP_ERROR    = 0x02U,  //错误
  FP_MISMATCH = 0x03U,  //指纹不匹配
  FP_TIMEOUT  = 0x04U   //执行超时
} FP_StatusTypeDef;

/* ================= 新增：非阻塞任务控制 ================= */
typedef enum {
    FP_IDLE = 0,      // 空闲
    FP_WAITING,       // 等待手指 (非阻塞让出CPU)
    FP_PROCESSING,    // 正在处理 (生成特征/比对中)
    FP_OK_NB,         // 非阻塞操作成功
    FP_MATCH_NB,      // 非阻塞匹配成功
    FP_MISMATCH_NB,   // 非阻塞不匹配
    FP_ERROR_NB       // 非阻塞错误
} FP_NonBlockStatus_t;

typedef struct {
    uint8_t step;             // 当前执行步骤
    uint16_t target_id;       // 目标存储ID
    SearchResult search_res;  // 搜索结果
} FP_Task_t;

extern uint16_t ValidN;//模块内有效模板个数

FP_StatusTypeDef FR_Init(void);   //指纹模块初始化

//---------------
FP_StatusTypeDef Add_FR(void);    //录入指纹
FP_StatusTypeDef Press_FR(void);  //刷指纹
uint8_t GZ_DeletChar(uint16_t PageID,uint16_t N);//删除模板，即是删除指定指纹 
uint8_t GZ_Empty(void);           //清空指纹库，删除所有指纹 
//---------------

// 以下函数为 AS608.c 内部使用 (static), 不在头文件声明
// uint8_t GZ_GetImage(void);
// uint8_t GZ_GenChar(uint8_t BufferID);
// uint8_t GZ_Match(void);
// uint8_t GZ_RegModel(void);
//static uint8_t GZ_WriteReg(uint8_t RegNum,uint8_t DATA);//写系统寄存器

uint8_t GZ_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);//搜索指纹 
uint8_t GZ_StoreChar(uint8_t BufferID,uint16_t PageID);//储存模板 
uint8_t GZ_ReadSysPara(SysPara *p); //读系统基本参数 
uint8_t GZ_SetAddr(uint32_t addr);  //设置模块地址 
uint8_t GZ_WriteNotepad(uint8_t NotePageNum,uint8_t *content);//写记事本 
uint8_t GZ_ReadNotepad(uint8_t NotePageNum,uint8_t *note);//读记事 
uint8_t GZ_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);//高速搜索 
uint8_t GZ_ValidTempleteNum(uint16_t *ValidN);//读有效模板个数 
uint8_t GZ_HandShake(uint32_t *GZ_Addr); //与AS608模块握手
//const char *EnsureMessage(uint8_t ensure);//确认码错误信息解析
//void GZ_StaGPIO_Init(void);//初始化PA6读状态引脚

// 非阻塞 API (主循环调用)
void FP_Task_Reset(void);
FP_NonBlockStatus_t FP_Task_Add(uint16_t target_id);
FP_NonBlockStatus_t FP_Task_Search(void);

#endif

