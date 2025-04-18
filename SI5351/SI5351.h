#include "stm32h7xx_hal.h"
#include "main.h"

//====================== Si5351寄存器地址定义 ======================//
#define SI_CLK0_CONTROL   16    // CLK0 控制寄存器
#define SI_CLK1_CONTROL   17    // CLK1 控制寄存器
#define SI_CLK2_CONTROL   18    // CLK2 控制寄存器
#define SI_SYNTH_PLL_A    26    // PLL A 合成器配置起始寄存器
#define SI_SYNTH_PLL_B    34    // PLL B 合成器配置起始寄存器
#define SI_SYNTH_MS_0     42    // Multisynth0 起始寄存器（用于输出0）
#define SI_SYNTH_MS_1     50    // Multisynth1 起始寄存器（用于输出1）
#define SI_SYNTH_MS_2     58    // Multisynth2 起始寄存器（用于输出2）
#define SI_PLL_RESET      177   // PLL复位寄存器

//====================== R分频器定义 ======================//
// 用于进一步分频输出频率（适用于低频输出）
#define SI_R_DIV_1        0x00
#define SI_R_DIV_2        0x10
#define SI_R_DIV_4        0x20
#define SI_R_DIV_8        0x30
#define SI_R_DIV_16       0x40
#define SI_R_DIV_32       0x50
#define SI_R_DIV_64       0x60
#define SI_R_DIV_128      0x70

//====================== PLL源选择 ======================//
#define SI_CLK_SRC_PLL_A  0x00  // 选择PLL A作为时钟源
#define SI_CLK_SRC_PLL_B  0x20  // 选择PLL B作为时钟源

//====================== 晶振频率设置 ======================//
#define XTAL_FREQ_deviation  2400  // 校准用晶振频偏（单位Hz）
#define XTAL_FREQ  (25000000 + XTAL_FREQ_deviation)  // 实际晶振频率（单位Hz），默认25MHz

//====================== GPIO定义 ======================//
// 软件IIC使用的SDA和SCL脚定义（可根据实际板子引脚更改）
#define Si5351_SDA_GPIO_Port GPIOB
#define Si5351_SDA_Pin       GPIO_PIN_6
#define Si5351_CLK_GPIO_Port GPIOB
#define Si5351_CLK_Pin       GPIO_PIN_7

//====================== 软件IIC模拟宏 ======================//
#define SDA(n)  { (n) ? HAL_GPIO_WritePin(Si5351_SDA_GPIO_Port, Si5351_SDA_Pin, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(Si5351_SDA_GPIO_Port, Si5351_SDA_Pin, GPIO_PIN_RESET); }

#define CLK(n)  { (n) ? HAL_GPIO_WritePin(Si5351_CLK_GPIO_Port, Si5351_CLK_Pin, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(Si5351_CLK_GPIO_Port, Si5351_CLK_Pin, GPIO_PIN_RESET); }




// 添加一个初始化函数：
void Si5351_Init(void); // 自动写入所有寄存器

// //====================== 函数声明 ======================//
// void SetPLLClk(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom);
// // 配置PLL（A或B），mult为整数倍数，num/denom为小数部分，用于生成目标频率

// void SetMultisynth(uint8_t synth, uint32_t divider, uint8_t rDiv);
// // 配置Multisynth（输出分频器），divider为整数倍分频，rDiv为R分频器值（用于输出低频）

// void SetFrequency(uint32_t frequency, uint8_t div, uint8_t Channel);
// // 设置指定通道（CLK0或CLK1）的输出频率

// void SetFrequencyDiv(uint32_t Frequency, uint8_t Channel);
// // 自动选择R分频器以支持较低输出频率的设置（如音频8kHz等）

// //====================== 外部变量 ======================//
// extern uint8_t Div;    // 当前R分频器选择
// extern uint8_t Multi;  // 频率倍数，用于反推频率
