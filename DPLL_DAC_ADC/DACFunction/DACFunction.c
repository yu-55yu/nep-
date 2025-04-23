#include "DACFunction.h"



uint16_t DAC_Buffer1[100];
uint16_t DAC_Buffer2[100];



//设置DAC输出波频率
void Frequency_Set(float Frequency,uint32_t Channel)//修改频率
{
	uint32_t ARR=2400000/100/Frequency-1;
	if(Channel == DAC_CHANNEL_1)
    {
		HAL_TIM_Base_Stop(&htim4);
		__HAL_TIM_SetAutoreload(&htim4, ARR);//设置1频率
		HAL_TIM_Base_Start(&htim4);
	}
   
	if(Channel == DAC_CHANNEL_2)
	{
	HAL_TIM_Base_Stop(&htim5);
	__HAL_TIM_SetAutoreload(&htim5, ARR);//设置2频率
	HAL_TIM_Base_Start(&htim5);
	}
}

//amp是幅值，范围0-3V
void Amp_Set(float Amp,uint32_t Channel,uint16_t *WaveData)//修改幅值
{
	
	float AmpWord = Amp/3000;
	for (int i = 0; i < 100; i++)
{
    float temp = WaveData[i] * AmpWord;
    WaveData[i] = (uint16_t)temp;
}

}


void Show_Waves(Signal Wave, uint32_t Channel)
{
    Frequency_Set(Wave.Freq/10, Channel);

    uint16_t *BaseWave = NULL;
    static uint16_t WaveBuffer[100];  // 使用静态缓冲区防止栈溢出

    if (Wave.Type == SINE)
        BaseWave = Sine12bit;
    else if (Wave.Type == TRIANGLE)
        BaseWave = TriangleWave;

    memcpy(WaveBuffer, BaseWave, sizeof(WaveBuffer));  // 复制波形数据https://www.msn.cn/zh-cn/news/other/%E6%B2%83%E5%B0%94%E6%B2%83%E6%96%B0%E8%BD%A6%E7%9B%B4%E9%99%8D-16-%E4%B8%87-%E5%AE%89%E5%85%A8%E9%85%8D%E7%BD%AE%E6%8B%89%E6%BB%A1-%E8%BF%99%E8%B0%81%E9%A1%B6%E5%BE%97%E4%BD%8F/ar-AA1DqwC3
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



