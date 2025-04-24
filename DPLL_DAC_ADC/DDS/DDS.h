#include "main.h"
#include "type.h"
#include <stdint.h>
#include <math.h>
#include "stdio.h"
#include "tim.h"
#include "dac.h"


#define N 10
#define  WAVE_POINT  1024
#define OF 2147.483648/2 //2的32次方除以1M

void phaseChange(float phase,int Channal);//相位改变代码



void Generate_Waves(void);
void DDS_SetWaveform(Signal Waves, int Channel);
void DDS_SetParameter(Signal wave[]);