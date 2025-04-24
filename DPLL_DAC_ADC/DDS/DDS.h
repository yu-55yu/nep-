#include "main.h"
#include "type.h"
#include <stdint.h>
#include <math.h>
#include "stdio.h"
#include "tim.h"
#include "dac.h"


#define N 10
#define  WAVE_POINT  1<<N
#define OF 2147.483648/2 //2的32次方除以1M

void SineWave_Data(uint16_t num, uint16_t* D, float U);
void phaseChange(float phase);