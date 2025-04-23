#include "DDS.h"


#define TABLE_SIZE 256
uint16_t sine_table[TABLE_SIZE];


#define F_CLK     1000000  // DAC触发频率 1MHz
#define TABLE_SIZE 256

volatile uint32_t phase_acc = 0;
volatile uint32_t phase_step = 0;
uint16_t waveform_table[TABLE_SIZE];  // 可选正弦/三角波表

void DDS_SetFrequency(uint32_t freq) 
{
    phase_step = ((uint64_t)freq << 32) / F_CLK;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4)
		{
        phase_acc += phase_step;
        uint16_t index = phase_acc >> (32 - 8); // 高8位查表
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, waveform_table[index]);
    }
}
