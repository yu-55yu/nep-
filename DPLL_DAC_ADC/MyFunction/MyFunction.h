#include "main.h"
#include "FFT.h" 
#include "DACFunction.h"
#include "type.h"
#include "DDS.h"


#define Fs 1000000 // ADC²ÉÑùÆµÂÊ

void Signal_Separation(void);
void Show_Waves(Signal Wave, uint32_t Channel);