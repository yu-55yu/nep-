#include "adcfunction.h"

#define ADC_LEN 1024
uint16_t ADC_Buffer[ADC_LEN];
double Volt_Buffer[ADC_LEN];
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim3;

#define PI 3.14159265358979f
float Window_OutputBuffer[ADC_LEN];                 //hanning窗 0~1024函数值
volatile uint8_t EnableWindow;                      //控制是否加窗
volatile uint8_t ChangeSamp = 0;
volatile uint8_t OverSamplingRate = 0;
float FFT_Input[2 * ADC_LEN] = {0};                //FFT输出
float FFT_Output[ADC_LEN];                         //FFT输入
float Fs = 200000;                              //采样率
uint16_t arr;
int BaseIdx;                                    //基波在FFT_Output数组里的索引
double HighV,LowV;
double Volt;

extern uint8_t ADCIRQflag;

float W = 0;

//采了1024个点
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) 
{
if (hadc==&hadc1) 
    {
        UNUSED(hadc);

        HAL_ADC_Stop(hadc);
				ADCIRQflag = 1;
    }
}

/*窗函数*/
void window(void)
{
    for(int i=0; i<ADC_LEN; i++)
    {
        if(EnableWindow)
        {
            float tempCos = cosf(2.0f * PI * i / (ADC_LEN - 1)); 
            Window_OutputBuffer[i] = 0.5f * (1.0f - tempCos);    // Hanning
        }
        else {
            Window_OutputBuffer[i]=1.0f;                                   //矩形窗
        }
    }
}


/*FFT+加窗*/
void FFT_Process(void)
{
    //加窗
    EnableWindow = 1;
    window();
    //转换为复数格式（虚部填 0）
    for (int i = 0; i < ADC_LEN; i++) {
        FFT_Input[2 * i] = Window_OutputBuffer[i]*ADC_Buffer[i];
        FFT_Input[2 * i + 1] = 0;
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_Input, 0, 1);//使用ARM CMSIS-DSP库的256点FFT函数 对ADC_Buffer中的原始采样数据进行复数FFT
    arm_cmplx_mag_f32(FFT_Input, FFT_Output, ADC_LEN);//计算复数FFT结果的幅度值（转换为实数频谱）
}


/* 找到基波的下标*/
void Find_BaseIndex(void)
{
    BaseIdx = 0;
    float max_val = 0;
    for (int i = 2; i < ADC_LEN / 2; i++) { // 遍历 0 ~ Fs/2 部分
        if (FFT_Output[i] > max_val) {
            max_val = FFT_Output[i];
            BaseIdx = i; // 记录基波的索引
        }
    }
}

void ChangeSamplingTime(void)
{
		ChangeSamp = (ChangeSamp+1)%4;
    if (ChangeSamp == 2) 
    {
        Find_WaveAmp(); // 计算幅值
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_Buffer, ADC_LEN); // 重新启动DMA
    }
    else if (ChangeSamp == 1)
    {
			 HAL_TIM_Base_Stop(&htim3); // 停止定时器
       FFT_Process(); // 处理FFT
       Find_BaseIndex(); // 找到基波的下标
       uint32_t ExFreq = (uint32_t)(Fs * BaseIdx / 1024.0);
       arr = 480000000/(ExFreq*256)-1;
			
       __HAL_TIM_SET_AUTORELOAD(&htim3,arr);

       HAL_TIM_Base_Start(&htim3); // 启动定时器
       HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_Buffer, ADC_LEN); // 重新启动DMA
    }
		 else if (ChangeSamp == 3)
    {
			 HAL_TIM_Base_Stop(&htim3); // 停止定时器 
       arr = 10000-1;
       __HAL_TIM_SET_AUTORELOAD(&htim3,arr);
       HAL_TIM_Base_Start(&htim3); // 启动定时器
       HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_Buffer, ADC_LEN); // 重新启动DMA
    }


}


void Find_WaveAmp(void)
{
    LowV = 1000000;
    HighV = 0;
    double VoltTemp = 0;
    for(int i =0;i<ADC_LEN;i++)
    {
        Volt_Buffer[i]=ADC_Buffer[i]*3.0/65535;
        if (Volt_Buffer[i] > HighV) {
            HighV = Volt_Buffer[i];
        }
        if (Volt_Buffer[i] < LowV) {
            LowV = Volt_Buffer[i];
        }
        VoltTemp+=fabs(Volt_Buffer[i]-1.65);
    }
    Volt = VoltTemp / ADC_LEN*1.4142135624; // 计算平均值
}