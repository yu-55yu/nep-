#include "main.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "math.h"
#include "type.h"

#define FFT_LEN 1024
#define ADC_LEN 1024



void window(void);
static float32_t transform_V(float32_t *p,int i);
void FFT_Process(void);
void ADC_FFT_Get_Wave_Mes(uint32_t Row,float Fs,float *VPP,float *Freq,int correctNum);

void Find_BaseIndex(void);
void Find_Harmonics(void);
void Find_2BaseIndex(void);
SignalType Detect_WaveType(int BaseIdx);
