//-----------------------------------------------------------------
// AD9910程序
// 头文件名: AD9910.h
// 作    者: 凌智电子
// 开始日期: 2019-12-1
// 完成日期: 2019-12-25
// 当前版本: V1.0
// 历史版本:
//-----------------------------------------------------------------
#include "main.h"
#include "Delay.h"
#ifndef _AD9910_H
#define _AD9910_H

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define AD9910_MRT_Set HAL_GPIO_WritePin(MRT_GPIO_Port, MRT_Pin, GPIO_PIN_SET)
#define AD9910_MRT_Clr HAL_GPIO_WritePin(MRT_GPIO_Port, MRT_Pin, GPIO_PIN_RESET)

#define AD9910_CSN_Set HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_SET)
#define AD9910_CSN_Clr HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_RESET)

#define AD9910_SCK_Set HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET)
#define AD9910_SCK_Clr HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET)

#define AD9910_SDI_Set HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_SET)
#define AD9910_SDI_Clr HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET)

#define AD9910_IUP_Set HAL_GPIO_WritePin(IUP_GPIO_Port, IUP_Pin, GPIO_PIN_SET)
#define AD9910_IUP_Clr HAL_GPIO_WritePin(IUP_GPIO_Port, IUP_Pin, GPIO_PIN_RESET)

#define AD9910_DRH_Set HAL_GPIO_WritePin(DRH_GPIO_Port, DRH_Pin, GPIO_PIN_SET)
#define AD9910_DRH_Clr HAL_GPIO_WritePin(DRH_GPIO_Port, DRH_Pin, GPIO_PIN_RESET)

#define AD9910_DRC_Set HAL_GPIO_WritePin(DRC_GPIO_Port, DRC_Pin, GPIO_PIN_SET)
#define AD9910_DRC_Clr HAL_GPIO_WritePin(DRC_GPIO_Port, DRC_Pin, GPIO_PIN_RESET)

#define AD9910_PF0_Set HAL_GPIO_WritePin(PF0_GPIO_Port, PF0_Pin, GPIO_PIN_SET)
#define AD9910_PF0_Clr HAL_GPIO_WritePin(PF0_GPIO_Port, PF0_Pin, GPIO_PIN_RESET)

#define AD9910_PF1_Set HAL_GPIO_WritePin(PF1_GPIO_Port, PF1_Pin, GPIO_PIN_SET)
#define AD9910_PF1_Clr HAL_GPIO_WritePin(PF1_GPIO_Port, PF1_Pin, GPIO_PIN_RESET)

#define AD9910_PF2_Set HAL_GPIO_WritePin(PF2_GPIO_Port, PF2_Pin, GPIO_PIN_SET)
#define AD9910_PF2_Clr HAL_GPIO_WritePin(PF2_GPIO_Port, PF2_Pin, GPIO_PIN_RESET)

#define AD9910_OSK_Set HAL_GPIO_WritePin(OSK_GPIO_Port, OSK_Pin, GPIO_PIN_SET)
#define AD9910_OSK_Clr HAL_GPIO_WritePin(OSK_GPIO_Port, OSK_Pin, GPIO_PIN_RESET)

// #define AD9910_TE_Set HAL_GPIO_WritePin(TE, GPIO_Pin_15, GPIO_PIN_SET)
// #define AD9910_TE_Clr HAL_GPIO_WritePin(GPIOA, GPIO_Pin_15, GPIO_PIN_RESET)

// #define AD9910_PC_Set HAL_GPIO_WritePin(GPIOA, GPIO_Pin_14, GPIO_PIN_SET)
// #define AD9910_PC_Clr HAL_GPIO_WritePin(GPIOA, GPIO_Pin_14, GPIO_PIN_RESET)


extern void GPIO_Init_AD9910(void);

extern void Write_8bit(u8 dat)	;
extern void Write_32bit(u32 dat)	;

extern void AD9910_Init(void);

extern void AD9910_Singal_Profile_Init(void);
extern void AD9910_Singal_Profile_Set(u8 addr,u32 Freq,u16 Amp ,u16 Pha);
extern void Set_Profile(u8 num);

extern void AD9910_Osk_Init(void);
extern void AD9910_Osk_Set(void);

extern void AD9910_DRG_Fre_Init(void);
extern void AD9910_DRG_Freq_set(u32 upper_limit , u32 lower_limit ,u32 dec_step , u32 inc_step , u16 neg_rate ,u16 pos_rate);

extern void AD9910_DRG_AMP_Init(void);
extern void AD9910_DRG_Amp_Set( u32 upper_limit , u32 lower_limit ,u32 dec_step , u32 inc_step , u16 neg_rate ,u16 pos_rate);

extern void AD9910_RAM_Init(void);
extern void AD9910_RAM_ZB_Fre_Init(void);
extern void AD9910_RAM_ZB_Fre_Set(u32 Freq);

extern void AD9910_RAM_Fre_W(void);
extern void AD9910_RAM_AMP_W(void);
extern void AD9910_WAVE_RAM_AMP_W(int mode);
extern void AD9910_RAM_DIR_Fre_R(void);
extern void AD9910_RAM_DIR_AMP_R(void);

extern void AD9910_RAM_RAMP_UP_ONE_Fre_R(void);
extern void AD9910_RAM_RAMP_UP_ONE_AMP_R(void);

extern void AD9910_RAM_RAMP_UP_TWO_Fre_R(void);
extern void AD9910_RAM_RAMP_UP_TWO_AMP_R(void);

extern void AD9910_RAM_BID_RAMP_Fre_R(void);
extern void AD9910_RAM_BID_RAMP_AMP_R(void);

extern void AD9910_RAM_CON_BID_RAMP_Fre_R(void);
extern void AD9910_RAM_CON_BID_RAMP_AMP_R(void);

extern void AD9910_RAM_CON_RECIR_Fre_R(void);
extern void AD9910_RAM_CON_RECIR_AMP_R(void);

extern void AD9910_DRG_Pha_Init(void);
extern void AD9910_DRG_Pha_Set( u32 upper_limit , u32 lower_limit ,u32 dec_step , u32 inc_step , u16 neg_rate ,u16 pos_rate);

extern void AD9910_RAM_Pha_W(void);
extern void AD9910_RAM_DIR_PHA_R(void);
extern void AD9910_RAM_RAMP_UP_ONE_PHA_R(void);
extern void AD9910_RAM_RAMP_UP_TWO_PHA_R(void);
extern void AD9910_RAM_BID_RAMP_PHA_R(void);
extern void AD9910_RAM_CON_BID_RAMP_PHA_R(void);
extern void AD9910_RAM_CON_RECIR_PHA_R(void);

// void Par_mod(u8 des ,u16 FF);
void AD9910_Init_Sin(int gain);
void Freq_convert(u32 Freq);
#endif
