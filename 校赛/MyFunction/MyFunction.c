#include "MyFunction.h"
#include "ti_msp_dl_config.h"

uint16_t ADC_Buffer[ADC_LEN];
extern float MaxV,MinV;
extern uint16_t Period;
float MaxVal,MinVal;

volatile uint8_t EnableWindow;                      //控制是否加窗
float Window_OutputBuffer[ADC_LEN];                 //hanning窗 0~1024函数值
float FFT_Input[FFT_LEN * 2];  
float FFT_Output[FFT_LEN];  
int BaseIdx;
extern float Fs;
uint8_t WaveType;//1方波 2 三角 3正弦
extern double Freq;
extern uint16_t Period;






void window(uint8_t EnableWindow)
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


void FFT_Process(uint16_t*ADC_Buff)
{
  for(int i=0;i<1024;i++)
    {
      FFT_Input[i * 2] = ADC_Buff[i] * Window_OutputBuffer[i];
      FFT_Input[i * 2 + 1] = 0;                    
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_Input, 0, 1);
    arm_cmplx_mag_f32(FFT_Input, FFT_Output, FFT_LEN); 
}


/* 找到基波的下标*/
void Find_BaseIndex(int *Idx)
{
    float max_val = 0;
    int base_idx = 0;
    for (int i = 2; i < FFT_LEN / 2; i++) { // 遍历 0 ~ Fs/2 部分
        if (FFT_Output[i] > max_val) {
            max_val = FFT_Output[i];
            base_idx = i; // 记录基波的索引
        }
    }
    *Idx = base_idx; // 返回结果
}


void ADC_FFT_Get_Wave_Mes(uint32_t Row,float Fs,float *VPP,float *Freq,int correctNum)
{
    int i;
    float k=2.667;                                     
    float DatePower1=0,DatePower2=0,f;
    for(i=-correctNum;i<=correctNum;i++)     
      {
          DatePower1+=(Row+i)*FFT_Output[Row+i]*FFT_Output[Row+i];
          DatePower2+=FFT_Output[Row+i]*FFT_Output[Row+i];
      }

      f=DatePower1/DatePower2;
      Freq[0] = f*Fs/FFT_LEN;
      VPP[0] = 2.0f*sqrtf(k*DatePower2)/4096.0*3.3;
}


void Find_MaxSegment_RangeWithin(
    const uint16_t *arr, int len,
    int max_diff,
    int *start_idx, int *end_idx)
{
    int max_len = 0;
    int left = 0;
    *start_idx = 0;
    *end_idx = 0;

    for (int right = 0; right < len; right++) {
        int min_val = arr[right], max_val = arr[right];

        for (int k = right; k >= left; k--) {
            if (arr[k] < min_val) min_val = arr[k];
            if (arr[k] > max_val) max_val = arr[k];
            if (max_val - min_val > max_diff) {
                left = k + 1;
                break;
            }
        }

        if (right - left + 1 > max_len) {
            max_len = right - left + 1;
            *start_idx = left;
            *end_idx = right;
        }
    }
}



void Find_MaxMin(float *Max, float *Min)
{
    float max_val = 0,max_idx=0;
    float min_val = 4095,min_idx=0;

    for (int i = 0; i < ADC_LEN; i++) {
        int val = ADC_Buffer[i];
        if (val > max_val){ 
            max_idx=i;
            max_val = val;
            }
        if (val < min_val) {
            min_idx=i;
            min_val = val;}
    }

    *Max = max_val * 3.3f / 4096.0f;
    *Min = min_val * 3.3f / 4096.0f;
}





// void Find_MaxMin(void)
// {
//     int cnt = ADC_LEN / Period;
//     int MaxVal_Temp = 0;
//     int MinVal_Temp = 0;
//     MaxVal_Temp=0,MinVal_Temp=0;

//     int *max = (int *)malloc(cnt * sizeof(int));
//     int *min = (int *)malloc(cnt * sizeof(int));

//     for (int i = 0; i < cnt; i++)
//     {
//         max[i] = 0;
//         min[i] = 4095;  
//     }

//     for (int i = 0; i < cnt; i++)
//     {
//         for (int j = 0; j < Period; j++)
//         {
//             int val = ADC_Buffer[i * Period + j];  
//             if (val > max[i])
//                 max[i] = val;
//             if (val < min[i])
//                 min[i] = val;
//         }
//     }
//     for (int i = 0; i < cnt; i++)
//     {
//         if(i>2)
//         {
//              max[i] = (max[i-2]+max[i-1]+max[i])/3.0;
//              min[i] = (min[i-2]+min[i-1]+min[i])/3.0;
//         }

//         MaxVal_Temp += max[i];
//         MinVal_Temp += min[i];
//     }

//     MaxVal_Temp = (float)MaxVal_Temp/ cnt ;
//     MinVal_Temp = (float)MinVal_Temp / cnt ;



//     for (int i = 0; i < cnt; i++)
//     {
//         if(i>2)
//         {
//             if(abs(max[i]-MaxVal_Temp)>30)max[i]=MaxVal_Temp;
//             max[i] = (max[i-2]+max[i-1]+max[i])/3.0;
//             min[i] = (min[i-2]+min[i-1]+min[i])/3.0;
//         }

//         MaxVal += max[i];
//         MinVal += min[i];
//     }

//     MaxVal = (float)(MaxVal / cnt);
//     MinVal = (float)(MinVal / cnt) ;
//     MaxV = MaxVal/ 4096 * 3.3;
//     MinV = MinVal/ 4096 * 3.3;

//     free(max);
//     free(min);
// }






// void Find_MaxMin(float *Max, float *Min)
// {
//     const int N = 1024;
//     const int W = 3;  // 滑动窗口大小

//     uint32_t P[N + 1];  // 前缀和数组：P[i] = ADC_Buffer[0] + ... + ADC_Buffer[i-1]
//     P[0] = 0;
//     for (int i = 1; i <= N; i++) {
//         P[i] = P[i - 1] + ADC_Buffer[i - 1];
//     }

//     float max_val = 0.0f;
//     float min_val = 3.3f;

//     for (int i = 0; i <= N - W; i++) {
//         uint32_t sum = P[i + W] - P[i];
//         float avg = sum / (float)W;
//         float voltage = avg * 3.3f / 4096.0f;

//         if (voltage > max_val) max_val = voltage;
//         if (voltage < min_val) min_val = voltage;
//     }

//     *Max = max_val;
//     *Min = min_val;
// }










void Find_Peaks(Peak *peaks, int baseIdx, float fs) {
    const int search_span = 3;
    int fft_half = FFT_LEN / 2;

    // --- 查找基波最大点（±search_span 内） ---
    float maxVal1 = 0.0f;
    int maxIdx1 = baseIdx;  // 默认使用传入 baseIdx

    for (int i = baseIdx - search_span; i <= baseIdx + search_span; i++) {
        if (i <= 1 || i >= fft_half - 1) continue;
        if (FFT_Output[i] > FFT_Output[i - 1] &&
            FFT_Output[i] > FFT_Output[i + 1] &&
            FFT_Output[i] > maxVal1) {
            maxVal1 = FFT_Output[i];
            maxIdx1 = i;
        }
    }



    // --- 三次谐波 ---
    int target3x = maxIdx1 * 3;
    int maxIdx3 = target3x;
    float maxVal3 = 0.0f;

    if (fs > 100 * Freq) {
        // 分辨率足够高，直接取 target3x 处作为三次谐波
        if (target3x < fft_half) {
            maxIdx3 = target3x;
            maxVal3 = FFT_Output[maxIdx3];
        }
    } else {
        // 分辨率不够，进行 ±search_span 范围搜索峰值
        for (int i = target3x - search_span; i <= target3x + search_span; i++) {
            if (i < 1 || i >= fft_half - 1) continue;
            if (FFT_Output[i] > FFT_Output[i - 1] &&
                FFT_Output[i] > FFT_Output[i + 1] &&
                FFT_Output[i] > maxVal3) {
                maxVal3 = FFT_Output[i];
                maxIdx3 = i;
            }
        }
    }
        if(maxIdx1>8){
        ADC_FFT_Get_Wave_Mes(maxIdx1,Fs,&peaks[0].value,&peaks[0].freq,2);
        ADC_FFT_Get_Wave_Mes(maxIdx3,Fs,&peaks[1].value,&peaks[1].freq,2);
    }
    else {
        peaks[0].index = maxIdx1;
        peaks[0].value = FFT_Output[maxIdx1];
        peaks[0].freq  = (float)maxIdx1 * fs / FFT_LEN;
          peaks[1].index = maxIdx3;
    peaks[1].value = FFT_Output[maxIdx3];
    peaks[1].freq  = (float)maxIdx3 * fs / FFT_LEN;
    }
}



void analyze_waveform(float fs) {
    Peak peaks[4];
    Find_Peaks(peaks,BaseIdx,fs);

    float f1 = peaks[0].freq;
    float f2 = peaks[1].freq;
    float a1 = peaks[0].value;
    float a2 = peaks[1].value;

    
    float k1 = fabs(3.0f*f1/f2 - 1);
    float m  = fabsf(a1/(3.0f * a2)-1);


if (k1 <= 0.3 && m <= 0.3f)
    WaveType = 1;//方波
else if (k1 <= 0.3 && m >= 1.0f&&m <= 2.5f)
    WaveType = 2;//三角*arr
else
    WaveType = 3;//正弦
}


//设采样率
void ChangSamp(float freq, float *fs, uint64_t *arr)
{
    // if (freq <= 20)
    // {
    //     *arr = 16000000/1024;
    //     *fs = 1024;  // ≈ 61.035 Hz
    // }
    // else if (freq < 40000)
    // {
    //     *fs =  50*freq;  // 每周期50个点
    //     *arr = (uint64_t)(16000000.0 / (*fs) - 0.5);
    // }
    // else
    // {
    //     *fs = 10*freq;  // 每周期10个点
    //     *arr = (uint64_t)(16000000.0 / (*fs) - 0.5);
    // }

      if (freq <= 20)
    {
        *arr = 16000000/1024;
        *fs = 1024;  // ≈ 61.035 Hz
    }
    else if (freq < 40000)
    {
        *fs = 100 * freq/11.0;  // 每周期50个点
        *arr = (uint64_t)(16000000.0 / (*fs) - 0.5);
    }
    else
    {
        *fs = 9* freq;  // 每周期10个点
        *arr = (uint64_t)(16000000.0 / (*fs) - 0.5);
    }
}


void ChangSampTriangle(float freq, float *fs, uint64_t *arr)
{
     if (freq <= 20)
    {
        *arr = 16000000/1024;
        *fs = 1024;  // ≈ 61.035 Hz
    }
    else if (freq < 40000)
    {
        *fs = 100 * freq/11.0;  // 每周期50个点
        *arr = (uint64_t)(16000000.0 / (*fs) - 0.5);
    }
    else
    {
        *fs = 9* freq;  // 每周期10个点
        *arr = (uint64_t)(16000000.0 / (*fs) - 0.5);
    }

}





/* 极值检测与拟合 */
void find_peak_valley(float* peak, float* valley) {
    int size = ADC_LEN-1;
    float MIN_PEAK_DIST = Fs/Freq/2;
    float max_val = 0;
    float min_val = 4096;
    float derivatives[ADC_LEN-1];
    
    // 计算差分
    for(int i=0; i<size-1; i++){
        derivatives[i] = ADC_Buffer[i+1] - ADC_Buffer[i];
    }

    // 寻找极值区域
    for(int i=1; i<size-1; i++){
        // 检测斜率符号变化
        if(derivatives[i]*derivatives[i-1] < 0){
            // 极值区域范围[i-1, i+1]
            float local_max = ADC_Buffer[i];
            float local_min = ADC_Buffer[i];
            
            // 在附近3个点找极值
            for(int j=-1; j<=1; j++){
                if(i+j <0 || i+j >= size) continue;
                if(ADC_Buffer[i+j] > local_max) local_max = ADC_Buffer[i+j];
                if(ADC_Buffer[i+j] < local_min) local_min = ADC_Buffer[i+j];
            }

            // 更新全局极值（需满足最小间距）
             int last_max_pos = -MIN_PEAK_DIST*2;
            int last_min_pos = -MIN_PEAK_DIST*2;
            
            if(derivatives[i] < 0 && (i - last_max_pos) > MIN_PEAK_DIST){
                if(local_max > max_val){
                    max_val = local_max;
                    last_max_pos = i;
                }
            }
            else if(derivatives[i] > 0 && (i - last_min_pos) > MIN_PEAK_DIST){
                if(local_min < min_val){
                    min_val = local_min;
                    last_min_pos = i;
                }
            }
        }
    }

    *peak = max_val;
    *valley = min_val;
}


