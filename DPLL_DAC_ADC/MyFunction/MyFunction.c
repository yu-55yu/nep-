#include "MyFunction.h"

extern uint16_t BaseIdx[2];
extern volatile uint8_t EnableWindow;   
extern uint16_t DAC_Buffer1[1024];
extern uint16_t DAC_Buffer2[1024];

extern const uint16_t Sine12bit[100];
extern const uint16_t TriangleWave[100];
extern uint16_t SquareWave[100];

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


/*
//设好波的所有参数
void Show_Waves(Signal Wave, uint32_t Channel)
{
    Frequency_Set(Wave.Freq/10, Channel);

    uint16_t *BaseWave = NULL;
    static uint16_t WaveBuffer[100];  // 使用静态缓冲区防止栈溢出

    if (Wave.Type == SINE)
        BaseWave = Sine12bit;
    else if (Wave.Type == TRIANGLE)
        BaseWave = TriangleWave;

    memcpy(WaveBuffer, BaseWave, sizeof(WaveBuffer));  // 复制波形数
    Amp_Set(Wave.VPP, Channel, WaveBuffer);            // 缩放幅值

	if(Channel == DAC_CHANNEL_1)
	{
		memcpy(DAC_Buffer1, WaveBuffer, 100*sizeof(uint16_t)); 
		//HAL_DAC_Start_DMA(&hdac1, Channel, (uint32_t *)DAC_Buffer1, 100, DAC_ALIGN_12B_R);
	}
	else if(Channel == DAC_CHANNEL_2)
	{
		memcpy(DAC_Buffer2, WaveBuffer, 100*sizeof(uint16_t)); 
		//HAL_DAC_Start_DMA(&hdac1, Channel, (uint32_t *)DAC_Buffer2, 100, DAC_ALIGN_12B_R);
	}
}
*/


