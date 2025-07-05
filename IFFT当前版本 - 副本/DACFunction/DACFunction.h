#include "main.h"
#include "tim.h"
#include "type.h"
#include "dac.h"
#include "string.h"
#include "stdio.h"


void Frequency_Set(float Frequency,uint32_t Channel);
void Show_Waves(Signal Wave,uint32_t Channel);
void Amp_Set(float Amp,uint32_t Channel,uint16_t *WaveData);