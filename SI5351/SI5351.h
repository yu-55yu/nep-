#ifndef __SI53531_H__
#define __SI53531_H__
#include "stm32h7xx_hal.h"
#include "main.h"
//#include "system.h"
//Si5351寄存器声明
#define SI_CLK0_CONTROL	16			// Register definitions
#define SI_CLK1_CONTROL	17
#define SI_CLK2_CONTROL	18
#define SI_SYNTH_PLL_A	26
#define SI_SYNTH_PLL_B	34
#define SI_SYNTH_MS_0		42
#define SI_SYNTH_MS_1		50
#define SI_SYNTH_MS_2		58
#define SI_PLL_RESET		177

#define SI_R_DIV_1		0x00			// R-division ratio definitions
#define SI_R_DIV_2		16				//0b00010000 //1Mh就要自己算 看手册
#define SI_R_DIV_4		32				//0b00100000
#define SI_R_DIV_8		48				//0b00110000
#define SI_R_DIV_16		64				//0b01000000
#define SI_R_DIV_32		80				//0b01010000
#define SI_R_DIV_64		96				//0b01100000
#define SI_R_DIV_128		112			//0b01110000

#define SI_CLK_SRC_PLL_A	0x00
#define SI_CLK_SRC_PLL_B	0x20  //  0b00100000 32    AC5的寄存器定义好像0b的写法会报错,但是AC6不会  


#define XTAL_FREQ_deviation 2400//矫正偏差
#define XTAL_FREQ	25000000+XTAL_FREQ_deviation	// Crystal frequency
//-3100 误差校正 0.00023



#define CLK(n) {n?HAL_GPIO_WritePin(SCL_GPIO_Port,SCL_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(SCL_GPIO_Port,SCL_Pin,GPIO_PIN_RESET);}
#define SDA(n) {n?HAL_GPIO_WritePin(SDA_GPIO_Port,SDA_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(SDA_GPIO_Port,SDA_Pin,GPIO_PIN_RESET);}
//相关函数声明
void SetPLLClk(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom);//设置PPL时钟
void SetFrequency(uint32_t frequency,uint8_t div,uint8_t Channel);//时钟Si5351时钟频率
void SetMultisynth(uint8_t synth,uint32_t divider,uint8_t rDiv);//设置多synth
void SetFrequencyDiv(uint32_t Frequency,uint8_t Channel);//调用锁相环控制时钟(最低8kHZ)
//相关变量声明
extern uint8_t Div;
extern uint8_t Multi;
#endif

