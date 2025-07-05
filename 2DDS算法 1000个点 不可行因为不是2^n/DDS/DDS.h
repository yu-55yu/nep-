#include "main.h"
#include "type.h"
#include <stdint.h>
#include <math.h>
#include "stdio.h"
#include "tim.h"
#include "dac.h"


// #define N 24 //32-10
#define  WAVE_POINT  1024 //4096点波形
#define OF 2147.483648*2 //2的32次方除以1M
#define PI 3.14159265358979f

void phaseChange(float phaseValue,int Channal);
void DDS_SetWaveform(Signal Waves, int Channel);
void DDS_SetParameter(Signal wave[]);