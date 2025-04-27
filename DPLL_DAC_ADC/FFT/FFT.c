#include "FFT.h"
#include "stdio.h"
#include "type.h"


#define PI 3.14159265358979f
volatile uint8_t EnableWindow;                      
float Window_OutputBuffer[ADC_LEN];                 
float FFT_Input[FFT_LEN * 2];  
float FFT_Output[FFT_LEN];
uint16_t ADC_Buffer[ADC_LEN];
uint16_t BaseIdx[2]={0};//这道题是两个波混一起 一般不用开数组 看题目


/**
  * @brief  Apply a window function (Hanning or Rectangular) to the output buffer.
  * @note   If EnableWindow is true, uses the Hanning window:
  *         w(n) = 0.5 * (1 - cos(2πn / (N - 1)))
  *         Otherwise, applies a rectangular window (all ones).
  * @param  None
  * @retval None
  */
void window(void)
{
    for (int i = 0; i < ADC_LEN; i++)
    {
        if (EnableWindow)
        {
            float tempCos = cosf(2.0f * PI * i / (ADC_LEN - 1)); 
            Window_OutputBuffer[i] = 0.5f * (1.0f - tempCos);    // Hanning
        }
        else
        {
            Window_OutputBuffer[i] = 1.0f;                       // 不加窗
        }
    }
}

/*
将采样值转化为真实的幅度
FFT_SIZE		FFT点数
4096 			代表12位ADC
3300			代表满量程3300mV
*/
static float32_t transform_V(float32_t *p,int i)
{
   if(i==0)
       return p[0]/FFT_LEN/65535.0*3000;
   else
       return p[0]*2.0f/FFT_LEN/65535.0*3000;
}



void FFT_Process(void)
{
  for(int i=0;i<1024;i++)
    {
      FFT_Input[i * 2] = ADC_Buffer[i] * Window_OutputBuffer[i];
      FFT_Input[i * 2 + 1] = 0;                    
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_Input, 0, 1);
    arm_cmplx_mag_f32(FFT_Input, FFT_Output, FFT_LEN);

		//for(int i=0;i<1024;i++)
		//{
		//	printf("%.2f\n",FFT_Output[i]);//这里打印了多少个就是多少个通道，上位机会识别

		//}
}


/*输入参数为FFT计算后的结果，输出矫正后的频率和幅度

Row				FFT结果中峰值的位置
Fs				采样频率
VPP[0]			矫正后的幅值
Freq[0]			矫正后的频率
correctNum		矫正的点数，一般取2即可，确保峰值左右的correctNum内没有其他信号
FFT_Output		FFT结果的幅值数组	
*/

void ADC_FFT_Get_Wave_Mes(uint32_t Row,float Fs,float *VPP,float *Freq,int correctNum)
{
	  int i;
    float k=2.667;                                     
    float DatePower1=0,DatePower2=0,f;
    for(i=-correctNum;i<=correctNum;i++)     
      {
          DatePower1+=(Row+i)*transform_V(FFT_Output+Row+i,Row+i)*transform_V(FFT_Output+Row+i,Row+i);
          DatePower2+=transform_V(FFT_Output+Row+i,Row+i)*transform_V(FFT_Output+Row+i,Row+i);
      }
      f=DatePower1/DatePower2;
      Freq[0] = f*Fs/FFT_LEN;
      VPP[0] = 2.0f*sqrtf(k*DatePower2);
}


//--------------------------------------不太用-------------------------------------------
// /* 找到基波的下标*/
// void Find_BaseIndex(void)
// {
//     BaseIdx = 0;
//     float max_val = 0;
//     for (int i = 2; i < FFT_LEN / 2; i++) { // 遍历 0 ~ Fs/2 部分
//         if (FFT_Output[i] > max_val) {
//             max_val = FFT_Output[i];
//             BaseIdx = i; // 记录基波的索引
//         }
//     }
// }

// /*  计算每个谐波的实际频率和幅值*/
// void Find_Harmonics(void)
// {   
//     // 计算基波及 2~5 次谐波的索引
//     for (int i = 0; i < 5; i++) {
//         Wave_Index[i] = BaseIdx * (i + 1);  // 计算谐波位置
//         if (Wave_Index[i] >= FFT_LEN / 2) break; // 避免超出 FFT 计算范围
//     }

//     // 计算每个谐波的实际频率和幅值
//     for (int i = 0; i < 5; i++) {
//         ADC_FFT_Get_Wave_Mes(Wave_Index[i], Fs, &VPP[i], &Freq[i], 2);
//     }
// }
//---------------------------------------不太用-------------------------------------------



//这个是具体题目的要求--------------------------------------------------------------------------------------
/* 找到幅值最大的两个频率分量下标（基波） */
/* 找到幅值最大的两个频率分量下标（基波），并确保它们至少相差 3 */
void Find_2BaseIndex(void)
{
    float max1 = 0, max2 = 0;
    int idx1 = 0, idx2 = 0;

    for (int i = 2; i < FFT_LEN / 2; i++) { // 遍历正频率部分，跳过直流和1号分量
        float val = FFT_Output[i];
        if (val > max1) {
            // 新的最大值，原 max1 移动到 max2
            max2 = max1;
            idx2 = idx1;
            max1 = val;
            idx1 = i;
        } else if (val > max2 && (i - idx1) >= 3) { // 确保 idx2 与 idx1 至少相差 3
            max2 = val;
            idx2 = i;
        }
    }
    BaseIdx[0] = idx1;
    BaseIdx[1] = idx2;
}

//判断是三角波还是正弦波
SignalType Detect_WaveType(int BaseIdx)
{
  int harmonic_count = 0;
  float threshold = FFT_Output[BaseIdx] * 0.01f;

  for (int i = BaseIdx; i < FFT_LEN / 2; i += (BaseIdx * 2)) {
      if (FFT_Output[i] > threshold) {
          harmonic_count++;
      }
  }

  if (harmonic_count >= 2) {
      return TRIANGLE;
  } else {
      return SINE;
  }
}



