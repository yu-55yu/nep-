#include "MyFunction.h"

extern uint16_t BaseIdx[2];
extern volatile uint8_t EnableWindow;   
extern uint16_t DAC_Buffer1[1024];
extern uint16_t DAC_Buffer2[1024];
extern uint16_t ADC_Buffer[ADC_LEN];


Signal Wave[2];


void Signal_Separation(void)
{
	EnableWindow=1;
    window();
    FFT_Process(ADC_Buffer);

    Find_2BaseIndex();
    for(int i=0; i<2; i++)
    {
        Wave[i].Type = Detect_WaveType(BaseIdx[i]);
        ADC_FFT_Get_Wave_Mes(BaseIdx[i], Fs, &Wave[i].VPP, &Wave[i].Freq, 3);
			
        //if(Wave[i].Type == TRIANGLE)
        //{
        //    Wave[i].VPP = Wave[i].VPP/0.8106;
        //}
    }
}
