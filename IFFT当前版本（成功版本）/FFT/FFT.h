#include "main.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "math.h"
#include "type.h"
#include "DDS.h"
#include <stdio.h>

#define FFT_LEN 1024
#define ADC_LEN 2048
#define Fs 1024000
#define DAC_LEN 2048
#define PI 3.14159265358979f            



void FFT_Process(uint16_t* adcbuffer);
void Find_2BaseIndex(void);
void Detect_WaveType(void);
void ProcessFFTData(uint8_t i,float phase);
void IFFT_Process(float *fft_data,uint16_t *dac_data);
float GetPhase(uint8_t idx);
void add_phase(double real_in, double imag_in, double delta_theta, double* real_out, double* imag_out);

