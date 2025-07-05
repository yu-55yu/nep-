#include "main.h"
#include "type.h"
#include <stdint.h>
#include <math.h>
#include "stdio.h"
#include "tim.h"
#include "dac.h"


#define N 11
#define  WAVE_POINT  2048 //4096点波形
#define OF 2147.483648*2/3 //2的32次方除以3M

// void phaseChange(float phase,int Channal);//相位改变代码

void DDS_SetWaveform(Signal Waves, int Channel);
void DDS_SetParameter(Signal wave[]);