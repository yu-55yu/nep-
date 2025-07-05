#include "FFT.h"
#include "main.h"
#include "stdio.h"

#define PI 3.14159265358979f
volatile uint8_t EnableWindow;                      
float Window_OutputBuffer[ADC_LEN];                 
float FFT_Input[FFT_LEN * 2];  
float FFT_Output[FFT_LEN];
uint16_t ADC_Buffer[ADC_LEN];
uint16_t ADC_Buffer1[ADC_LEN];
uint16_t ADC_Buffer2[ADC_LEN];
uint16_t BaseIdx[2]={0};
extern Signal Wave[2];

    float phase_adc[2] = {0};//ADC
    float phase_dac[2] = {0};//DAC


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



void FFT_Process(uint16_t* adcbuffer)
{
  for(int i=0;i<1024;i++)
    {
      FFT_Input[i * 2] = adcbuffer[i];
      FFT_Input[i * 2 + 1] = 0;                    
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_Input, 0, 1);
    arm_cmplx_mag_f32(FFT_Input, FFT_Output, FFT_LEN);

		//for(int i=0;i<1024;i++)
		//{
		//	printf("%.2f\n",FFT_Output[i]);//这里打印了多少个就是多少个通道，上位机会识别

		//}
}



void ADC_FFT_Get_Wave_Mes(uint32_t Row,float fs,float *VPP,float *Freq,int correctNum)
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
      Freq[0] = f*fs/FFT_LEN;
      VPP[0] = 2.0f*sqrtf(k*DatePower2);
}




void Find_2BaseIndex(void)
{
    float max1 = 0, max2 = 0;
    int idx1 = 0, idx2 = 0;

    // 先找最大值
    for (int i = 3; i < FFT_LEN / 2; i++) {
        if (FFT_Output[i] > max1) {
            max1 = FFT_Output[i];
            idx1 = i;
        }
    }

    // 再找与最大值下标相差至少3的次大值
    for (int i = 3; i < FFT_LEN / 2; i++) {
        if (i >= idx1 - 2 && i <= idx1 + 2) continue; // 跳过与最大值太近的
        if (FFT_Output[i] > max2) {
            max2 = FFT_Output[i];
            idx2 = i;
        }
    }

    BaseIdx[0] = idx1;
    BaseIdx[1] = idx2;
}

SignalType Detect_WaveType(int baseidx)
{
    float base = FFT_Output[baseidx];
    int third_idx = baseidx * 3;
    int fifth_idx = baseidx * 5;
    float third = 0.0f, fifth = 0.0f;
    if (third_idx < FFT_LEN / 2) third = FFT_Output[third_idx];
    if (fifth_idx < FFT_LEN / 2) fifth = FFT_Output[fifth_idx];

    int other_idx = (BaseIdx[0] == baseidx) ? BaseIdx[1] : BaseIdx[0];

    if (other_idx == baseidx * 3 || baseidx == other_idx * 3) {
        // 三倍关系，判断五次谐波
        float ratio5 = (base > 1e-6f) ? (fifth / base) : 0.0f;
        if (ratio5 > 0.04f && ratio5 < 0.10f) {
            return TRIANGLE;
        } else {
            return SINE;
        }
    } else {
        // 正常判断三次谐波
        float ratio3 = (base > 1e-6f) ? (third / base) : 0.0f;
        if (ratio3 > 0.08f && ratio3 < 0.15f) {
            return TRIANGLE;
        } else {
            return SINE;
        }
    }
}

float GetPhase(uint16_t *adcbuffer)
{
    FFT_Process(adcbuffer);
    int max_idx = 2;
    float max_val = FFT_Output[2];
    for (int i = 3; i < FFT_LEN / 2; i++) {
        if (FFT_Output[i] > max_val) {
            max_val = FFT_Output[i];
            max_idx = i;
        }
    }
    float real = FFT_Input[2 * max_idx];
    float imag = FFT_Input[2 * max_idx + 1];
    float phase = atan2f(imag, real);
    return phase/(2*PI)*360;
}


void GetPhaseDiff(void) {

    FFT_Process(ADC_Buffer);
    for(int i = 0; i < 2; i++) {
        uint16_t idx = BaseIdx[i];
        if(idx!= 0) {
            float real = FFT_Input[2 * idx];
            float imag = FFT_Input[2 * idx + 1];
            phase_adc[i] = atan2f(imag, real);
        }
    }
    FFT_Process(ADC_Buffer1);
    float real = FFT_Input[2 * BaseIdx[0]];
    float imag = FFT_Input[2 * BaseIdx[0] + 1];
    phase_dac[0] = atan2f(imag, real);
    // 计算相位差并归一化到[-π, π]
    float dphi = phase_adc[0] - phase_dac[0];
    if (dphi > PI) dphi -= 2 * PI;
    if (dphi < -PI) dphi += 2 * PI;

    Wave[0].PhasePast = Wave[0].Phase;
    Wave[0].Phase = dphi;	
 //   printf("Phase ADC:- %d, Phase DAC: %d, Phase Diff: %d\n", (int)(phase_adc[0]/(2*PI)*360), (int)(phase_adc[0]/(2*PI)*360),(int)(phase_adc[0]/(2*PI)*360));
}

void GetFreq(void)
{
    if(Wave[0].PhasePast!=0)
    {
 printf("%d,%d\n",(int)(Wave[0].PhasePast/(2*PI)*360000),(int)(Wave[0].Phase/(2*PI)*360000));

        double dphi = Wave[0].Phase - Wave[0].PhasePast;
        if (dphi > PI) dphi -= 2*PI;
        if (dphi < -PI) dphi += 2*PI;
        Wave[0].Freq = Wave[0].Freq - (dphi/(2*PI));
        Wave[0].Phase=0;
        Wave[0].PhasePast=0;
    }
    if(Wave[1].PhasePast!=0)
    {
        double dphi = Wave[1].Phase - Wave[1].PhasePast;
        if (dphi > PI) dphi -= 2*PI;
        if (dphi < -PI) dphi += 2*PI;
        Wave[1].Freq = Wave[1].Freq - (dphi/(2*PI));
        Wave[1].Phase=0;
        Wave[1].PhasePast=0;
    }
}
