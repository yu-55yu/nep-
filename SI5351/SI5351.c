#include "si5351.h"
#include "Si5351A-RevB-Registers.h"

uint8_t Div;
uint8_t Multi;

#define SYSCLK 480  // 系统时钟（单位：MHz）用于 delay_us 计算

// 精确延时（单位：微秒）
void HAL_Delay_us(uint32_t nus)
{
    uint32_t ticks = nus * SYSCLK;
    uint32_t told = SysTick->VAL;
    uint32_t tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += reload - tnow + told;
            told = tnow;
            if (tcnt >= ticks)
                break;
        }
    }
}

// 用户调用的微秒延时接口
void delay_us1(uint16_t n)
{
    HAL_Delay_us(n);
}

// 启动 I2C 通信
void IICstart()
{
    SDA(1);
    CLK(1);
    SDA(0);
    delay_us1(100);
    CLK(0);
    delay_us1(100);
}

// 停止 I2C 通信
void IICstop()
{
    CLK(0);
    delay_us1(100);
    SDA(0);
    delay_us1(100);
    CLK(1);
    delay_us1(100);
    SDA(1);
    delay_us1(100);
}

// 发送一个字节
void IICsend(uint8_t DATA)
{
    for (int i = 0; i < 8; i++)
    {
        CLK(0);
        SDA((DATA & 0x80) ? 1 : 0);
        DATA <<= 1;
        delay_us1(100);
        CLK(1);
        delay_us1(100);
    }

    CLK(0);
    SDA(1);  // 释放 SDA
    delay_us1(100);
    CLK(1);  // 读取 ACK
    delay_us1(100);
    CLK(0);
}

// 向指定寄存器写一个值
void IICsendreg(uint8_t reg, uint8_t data)
{
    IICstart();
    delay_us1(200);
    IICsend(0xC0);  // Si5351 写地址（0x60 << 1）
    delay_us1(200);
    IICsend(reg);
    delay_us1(200);
    IICsend(data);
    delay_us1(200);
    IICstop();
}



void Si5351_Init(void) {
    for(int i = 0; i < SI5351A_REVB_REG_CONFIG_NUM_REGS; i++) {
        IICsendreg(si5351a_revb_registers[i].address, si5351a_revb_registers[i].value);
    }
    // 开启输出（可选）
    IICsendreg(3, 0x00);  // 使能所有输出
}



// // 设置 PLL 频率
// void SetPLLClk(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom)
// {
//     uint32_t P1, P2, P3;

//     P1 = (uint32_t)(128 * mult + 128 * ((float)num / denom) - 512);
//     P2 = (uint32_t)(128 * num - denom * ((float)128 * num / denom));
//     P3 = denom;

//     IICsendreg(pll + 0, (P3 >> 8) & 0xFF);
//     IICsendreg(pll + 1, P3 & 0xFF);
//     IICsendreg(pll + 2, (P1 >> 16) & 0x03);
//     IICsendreg(pll + 3, (P1 >> 8) & 0xFF);
//     IICsendreg(pll + 4, P1 & 0xFF);
//     IICsendreg(pll + 5, ((P3 >> 12) & 0xF0) | ((P2 >> 16) & 0x0F));
//     IICsendreg(pll + 6, (P2 >> 8) & 0xFF);
//     IICsendreg(pll + 7, P2 & 0xFF);
// }

// // 设置分频器输出
// void SetMultisynth(uint8_t synth, uint32_t divider, uint8_t rDiv)
// {
//     uint32_t P1 = 128 * divider - 512;
//     uint32_t P2 = 0;
//     uint32_t P3 = 1;

//     IICsendreg(synth + 0, (P3 >> 8) & 0xFF);
//     IICsendreg(synth + 1, P3 & 0xFF);
//     IICsendreg(synth + 2, ((P1 >> 16) & 0x03) | rDiv);
//     IICsendreg(synth + 3, (P1 >> 8) & 0xFF);
//     IICsendreg(synth + 4, P1 & 0xFF);
//     IICsendreg(synth + 5, ((P3 >> 12) & 0xF0) | ((P2 >> 16) & 0x0F));
//     IICsendreg(synth + 6, (P2 >> 8) & 0xFF);
//     IICsendreg(synth + 7, P2 & 0xFF);
// }

// // 设置指定通道的频率（核心函数）
// void SetFrequency(uint32_t frequency, uint8_t div, uint8_t Channel)
// {
//     uint32_t xtalFreq = XTAL_FREQ;  // 默认晶振频率
//     uint32_t pllFreq = (900000000 / frequency) * frequency;
//     if ((900000000 / frequency) % 2) pllFreq -= frequency; // 降到偶数倍

//     uint8_t mult = pllFreq / xtalFreq;
//     uint32_t l = pllFreq % xtalFreq;
//     uint32_t num = (uint32_t)((float)l * 1048575 / xtalFreq);
//     uint32_t denom = 1048575;

//     if (Channel == 0)
//     {
//         SetPLLClk(SI_SYNTH_PLL_A, mult, num, denom);
//         SetMultisynth(SI_SYNTH_MS_0, 900000000 / frequency, div);
//         IICsendreg(SI_PLL_RESET, 0xA0);
//         IICsendreg(SI_CLK0_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);
//     }
//     else if (Channel == 1)
//     {
//         SetPLLClk(SI_SYNTH_PLL_B, mult, num, denom);
//         SetMultisynth(SI_SYNTH_MS_1, 900000000 / frequency, div);
//         IICsendreg(SI_PLL_RESET, 0xA0);
//         IICsendreg(SI_CLK1_CONTROL, 0x4F | SI_CLK_SRC_PLL_B);
//     }
// }

// // 根据频率范围自动设置 rDiv 和 multiplier，再设置频率
// void SetFrequencyDiv(uint32_t Frequency, uint8_t Channel)
// {
//     if (Frequency > 1000000) {
//         Div = SI_R_DIV_1;  Multi = 1;
//     } else if (Frequency > 500000) {
//         Div = SI_R_DIV_2;  Multi = 2;
//     } else if (Frequency > 250000) {
//         Div = SI_R_DIV_4;  Multi = 4;
//     } else if (Frequency > 125000) {
//         Div = SI_R_DIV_8;  Multi = 8;
//     } else if (Frequency > 62500) {
//         Div = SI_R_DIV_16; Multi = 16;
//     } else if (Frequency > 31250) {
//         Div = SI_R_DIV_32; Multi = 32;
//     } else if (Frequency > 15625) {
//         Div = SI_R_DIV_64; Multi = 64;
//     } else {
//         Div = SI_R_DIV_128; Multi = 128;
//     }
//     SetFrequency(Frequency * Multi, Div, Channel);
// }
