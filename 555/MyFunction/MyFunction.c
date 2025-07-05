#include "MyFunction.h"

extern uint16_t BaseIdx[2];
extern volatile uint8_t EnableWindow;   
extern uint16_t DAC_Buffer1[1024];
extern uint16_t DAC_Buffer2[1024];
extern uint16_t ADC_Buffer[ADC_LEN];
extern float FFT_Output[FFT_LEN];


Signal Wave[2];


void Signal_Separation(void)
{
    FFT_Process(ADC_Buffer);
    Find_2BaseIndex();
    for(int i=0; i<2; i++)
    {
        Wave[i].Type = Detect_WaveType(BaseIdx[i]);
        Wave[i].Freq = BaseIdx[i]*1000;
    }
}
