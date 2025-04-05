#include <ti/iqmath/include/IQmathLib.h>
#include "arm_math.h"
#include "arm_const_structs.h"
#include <stdio.h>
#define ADC_LEN 1024
#define FFT_SIZE 1024



void window(void);
void ADC_FFT_Get_Wave_Mes(uint32_t Row,float Fs,float *VPP,float *Freq,int correctNum);
static float32_t transform_V(float32_t *p,int i);
void FFT_Process(void);
void Find_Harmonics(void);
void Find_BaseIndex(void);
void THD_Process(void);
void Normalized_Amp(void);
void ChangeSamplingTime(void);
void delay_ms(uint32_t ms);
void Show_Waveforms(void);
void Process(void);
void Show_Waveforms(void);
void Show_Parameters(void);
void Interpolate_array(void);