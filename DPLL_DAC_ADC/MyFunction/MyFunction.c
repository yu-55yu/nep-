#include "MyFunction.h"



extern uint16_t BaseIdx[2];
extern volatile uint8_t EnableWindow;   



Signal Wave[2];

//对ADC数组的数据FFT，找到两个波（要判断三角波、正弦波）需要找到两个基波（幅值相同） 判断是否有谐波（奇数次 1/n方）
void Signal_Separation(void)
{
    // 1. Apply window function to the ADC buffer
	  EnableWindow=1;
    window();
    // 2. Perform FFT on the windowed data
    FFT_Process();
    
    // 3. Process the FFT output to find the desired signals
    // (This part is not implemented in the original code, but you can add your own logic here)
    Find_2BaseIndex();
    for(int i=0; i<2; i++)
    {
        Wave[i].Type = Detect_WaveType(BaseIdx[i]);
        ADC_FFT_Get_Wave_Mes(BaseIdx[i], Fs, &Wave[i].VPP, &Wave[i].Freq, 3);
			if(Wave[i].Type == TRIANGLE)
			{
				Wave[i].VPP = Wave[i].VPP/0.8106;
			}
    }
    
}


