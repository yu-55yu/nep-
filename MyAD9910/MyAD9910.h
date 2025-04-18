#include "main.h"
#include "memorymap.h"
#include "gpio.h"
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

void Delay_ns(u8 t);				//延时函数
void Write_8bit(u8 dat);		//写8位数据
void Write_32bit(u32 dat);	//写32位数据

void AD9910_Init(void);			//AD9910初始化

void AD9910_Singal_Profile_Set(u8 addr, u32 Freq, u16 Amp, u16 Pha);//AD9910单频模式
void Set_Profile(u8 num);		//AD9910的Profile选择

void AD9910_DRG_Freq_set(uint8_t enable,u32 upper_limit, u32 lower_limit, u32 dec_step, u32 inc_step, u16 neg_rate, u16 pos_rate);
//AD9910数字斜坡扫频AD9910_DRC_Set;需要拉高这个引脚来触发向上扫,循环扫频则无所谓
void AD9910_DRG_Amp_Set(uint8_t enable, u32 upper_limit, u32 lower_limit, u32 dec_step, u32 inc_step, u16 neg_rate, u16 pos_rate);
//AD9910数字斜坡扫幅AD9910_DRC_Set;需要拉高这个引脚来触发向上扫,循环扫频则无所谓

typedef enum {
	TRIG_WAVE = 0,
	SQUARE_WAVE,
	SINC_WAVE,
	SINE_WAVE,
} AD9910_WAVE_ENUM;

void AD9910_RAM_AMP_WAVE_Set(AD9910_WAVE_ENUM wave);
//设置AD9910,RAM幅度模式，向AD9910芯片内部RAM写入1024个点的波形数据，使模块可输出任意波形
//但是输出的幅值只有0-360mV左右
void AD9910_RAM_PLO_WAVE_Set(AD9910_WAVE_ENUM wave);
//设置AD9910,RAM极性模式，向AD9910芯片内部RAM写入1024个点的波形数据，使模块可输出任意波形
//但是输出的幅值只有0-720mV左右
