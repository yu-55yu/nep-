#include <stdio.h>
#include "arm_math.h"
#include "arm_const_structs.h"
#include "math.h"
#include "Print.h"
#include <stdlib.h>
#include "BlueTooth.h"
#include "Music.h"
#include <stdint.h>  

#define ADC_LEN 1024
#define FFT_LEN 1024


#define PI 3.14159265358979f

typedef struct {
    int index;       // 峰值在数组中的索引
    float value;     // 峰值幅值
    float freq;      // 峰值对应频率，自动计算
} Peak;


void window(uint8_t EnableWindow);
void FFT_Process(uint16_t*ADC_Buff);
void Find_BaseIndex(int *Idx);
void Find_MaxMin(float *Max, float *Min);
// void Find_Peaks(Peak *peaks,uint64_t fs);
void Find_Peaks(Peak *peaks, int baseIdx, float fs);
void analyze_waveform(float fs);
void ADC_FFT_Get_Wave_Mes(uint32_t Row,float Fs,float *VPP,float *Freq,int correctNum);
void ChangSamp(float freq, float *fs, uint64_t *arr);
// void Find_MaxMin(void);
void Find_MaxSegment_RangeWithin(const uint16_t *arr, int len,int max_diff,int *start_idx, int *end_idx);
void find_peak_valley(float* peak, float* valley);
void ChangSampTriangle(float freq, float *fs, uint64_t *arr);