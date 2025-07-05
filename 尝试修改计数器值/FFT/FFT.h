#include "main.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "math.h"
#include "type.h"

#define FFT_LEN 1024
#define ADC_LEN 1024
#define WAVE_POINT 1000



void fft_cal_2types(void);
SignalType check_waveform_by_harmonic(double* mag, int base_freq_khz, int harmonic_order);
double modulus(double I, double Q);
double corr1000_200(uint16_t* data, uint16_t* mask);
void cal_2frqs(void);
void Prepare_DAC_Buffer(void);
void Update_All_DAC_Delays(void);
uint16_t Calculate_Phase_Correction_Ticks(double sin_comp, double cos_comp, int frq_index);
