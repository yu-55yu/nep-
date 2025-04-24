#include "DDS.h"

uint16_t pData[WAVE_POINT]={0};
uint16_t Sine_WAVE[WAVE_POINT];
uint32_t place=0;
uint32_t ctrl_word=0;
uint32_t phase_word=0;
uint8_t phaseFlag=1;

void SineWave_Data(uint16_t num, uint16_t* D, float U)//正弦波生成波表代码
{
 	num++;
     uint16_t i;
     U=U/2;
     for (i = 0; i < num-1; i++)
     {
        D[i] = (uint16_t)((U*sin((1.0 * i / (num - 1)) * 2 * 3.1415926) + U) * 4095 / 3.3)+300;
     }
}	

void phaseChange(float phase)//相位改变代码
{
 	phase_word=phase/360*4294967295;
 	if(phaseFlag==1)
 	{
 		phaseFlag=0;
 		place+=phase_word;
 	}
}
