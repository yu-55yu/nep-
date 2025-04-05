#include "MyFunction.h"

uint16_t ADC_Buffer[ADC_LEN];                       //ADC采集（0~4095）
float FFT_Input[2 * FFT_SIZE] = {0};                //FFT输出
float FFT_Output[FFT_SIZE];                         //FFT输入

//基波+2~5次谐波的参数
float VPP[5];                                   // 幅值（原始数据FFT）
float Freq[5];                                  //频率
int Wave_Index[5];                              //索引

float Window_OutputBuffer[ADC_LEN];                 //hanning窗 0~1024函数值
volatile uint8_t EnableWindow;                      //控制是否加窗
volatile uint8_t ChangeSamp = 1;
volatile uint8_t OverSamplingRate = 0;

float Fs = 200000;                              //采样率
uint16_t arr;
int BaseIdx;                                    //基波在FFT_Output数组里的索引

float THD;



#define NUM_OUTPUT_POINTS 310 // 插值后输出数据点数量
float bufferout[NUM_OUTPUT_POINTS];



/*窗函数*/
void window(void)
{
    for(int i=0; i<ADC_LEN; i++)
    {
        if(EnableWindow)
        {
            _iq tempCos = _IQcos(_IQ(2*PI*i/(ADC_LEN-1)));
            Window_OutputBuffer[i]=0.5*(1-_IQtoF(tempCos));             //hanning
        }
        else {
            Window_OutputBuffer[i]=1;                                   //矩形窗
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
       return p[0]/FFT_SIZE/4096.0*3300;
   else
       return p[i]*2.0f/FFT_SIZE/4096.0*3300;
}


/*
能量中心矫正
输入参数为FFT计算后的结果，输出矫正后的频率和幅度
Row				FFT结果中峰值的位置
Fs				采样频率
VPP[0]			矫正后的幅值
Freq[0]			矫正后的频率
correctNum		矫正的点数，一般取2即可，确保峰值左右的correctNum内没有其他信号
FFT_Output		FFT结果的幅值数组	
*/
void ADC_FFT_Get_Wave_Mes(uint32_t Row,float Fs,float *VPP,float *Freq,int correctNum)
{
    float k=2.667;                                     
    float DatePower1=0,DatePower2=0,f;
    for(int i=-correctNum;i<=correctNum;i++)     
      {
          DatePower1+=(Row+i)*FFT_Output[Row+i]*FFT_Output[Row+i];
          DatePower2+=FFT_Output[Row+i]*FFT_Output[Row+i];
      }
      f=DatePower1/DatePower2;
      Freq[0] = f*Fs/FFT_SIZE;
      VPP[0] = 2.0f*sqrtf(k*DatePower2);
}

//输出实际的电压 A题按比例算用不着
// void ADC_FFT_Get_Wave_Mes(uint32_t Row,float Fs,float *VPP,float *Freq,int correctNum)
// {
//     float k=2.667;                                     
//     float DatePower1=0,DatePower2=0,f;
//     int i;
//     for(i=-correctNum;i<=correctNum;i++)     
//       {
//           DatePower1+=(Row+i)*transform_V(FFT_Output+Row+i,Row+i)*transform_V(FFT_Output+Row+i,Row+i);
//           DatePower2+=transform_V(FFT_Output+Row+i,Row+i)*transform_V(FFT_Output+Row+i,Row+i);
//       }
//       f=DatePower1/DatePower2;
//       Freq[0] = f*Fs/FFT_SIZE;
//       VPP[0] = 2.0f*sqrtf(k*DatePower2);
// }



/*FFT+加窗*/
void FFT_Process(void)
{
    //加窗
    EnableWindow = 1;
    window();
    //转换为复数格式（虚部填 0）
    for (int i = 0; i < FFT_SIZE; i++) {
        FFT_Input[2 * i] = Window_OutputBuffer[i]*ADC_Buffer[i];
        FFT_Input[2 * i + 1] = 0;
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_Input, 0, 1);//使用ARM CMSIS-DSP库的256点FFT函数 对ADC_Buffer中的原始采样数据进行复数FFT
    arm_cmplx_mag_f32(FFT_Input, FFT_Output, FFT_SIZE);//计算复数FFT结果的幅度值（转换为实数频谱）
}


/* 找到基波的下标*/
void Find_BaseIndex(void)
{
    BaseIdx = 0;
    float max_val = 0;
    for (int i = 2; i < FFT_SIZE / 2; i++) { // 遍历 0 ~ Fs/2 部分
        if (FFT_Output[i] > max_val) {
            max_val = FFT_Output[i];
            BaseIdx = i; // 记录基波的索引
        }
    }
}



/*  计算每个谐波的实际频率和幅值*/
void Find_Harmonics(void)
{
   
    // 计算基波及 2~5 次谐波的索引
    for (int i = 0; i < 5; i++) {
        Wave_Index[i] = BaseIdx * (i + 1);  // 计算谐波位置
        if (Wave_Index[i] >= FFT_SIZE / 2) break; // 避免超出 FFT 计算范围
    }

    // 计算每个谐波的实际频率和幅值
    for (int i = 0; i < 5; i++) {
        ADC_FFT_Get_Wave_Mes(Wave_Index[i], Fs, &VPP[i], &Freq[i], 2);
    }
}


//计算THD
void THD_Process(void)
{
    float result = 0;
    for(int i=1; i<5; i++)
    {
        result =  result + VPP[i]*VPP[i];
    }
    THD = sqrtf(result)/VPP[0];
}

//归一化幅值
void Normalized_Amp(void)
{
    for(int i=1; i<5; i++)
    {
        VPP[i] = VPP[i]/VPP[0];
        if(VPP[i]-0.08<0) VPP[i] = 0;
    }
        VPP[0] = 1;
}


void ChangeSamplingTime(void)
{
    if (ChangeSamp == 1)  // 第一次修改采样率：将基波移到 FFT_Output[80]
    {
        if (BaseIdx != 0)
        {
        arr = (int)(6400000000.0/Fs/BaseIdx -1);
        Fs =(int)( 80000000.0/(arr +1)+0.5);
        }
    }
    else if (ChangeSamp == 2)  // 第二次修改采样率：让一个周期采样400点
    {
        Fs = Fs*80*(NUM_OUTPUT_POINTS-10)/1024;
        if(Fs>2000000)
        {
            // Fs=Fs/2;
            Fs = 2000000;
            OverSamplingRate = 1;
        }
        arr = (uint16_t)(80000000.0 / Fs - 1);
    }


}

void Process(void)
{
    if(ChangeSamp == 1)
    {
        ADC_FFT_Get_Wave_Mes(BaseIdx, Fs, &VPP[0], &Freq[0], 3);  // 精确求基波频率
        ChangeSamplingTime(); // Fs = Freq[0] * FFT_SIZE / 80
    }
    else if(ChangeSamp == 2)
    {
        Find_Harmonics();        // 计算 2~5 次谐波
        THD_Process();           // THD
        Normalized_Amp();        // 归一化幅值
        Show_Parameters();       // 参数显示
        ChangeSamplingTime(); 
    }
    ChangeSamp = (ChangeSamp + 1) % 3;
}
  
void Show_Waveforms(void)
{
    printf("addt s0.id,0,310\xff\xff\xff");
    //等待适量时间
    delay_ms(100);

    if(!OverSamplingRate)
    for(int i = 0; i < NUM_OUTPUT_POINTS; i++)
    {
        printf("%c", (int)(ADC_Buffer[i]/20 + 10));
    }
    else
    {
        Interpolate_array();
        for(int i = 0; i < NUM_OUTPUT_POINTS; i++)
        {
            printf("%c", (int)(bufferout[i]/20+10));
        }
    }
    
    printf("\x01\xff\xff\xff");
}

void Show_Parameters(void)
{
    printf("t6.txt=\"%.2f\"\xff\xff\xff",VPP[0]);
    printf("t7.txt=\"%.2f\"\xff\xff\xff",VPP[1]);
    printf("t8.txt=\"%.2f\"\xff\xff\xff",VPP[2]);
    printf("t9.txt=\"%.2f\"\xff\xff\xff",VPP[3]);
    printf("t10.txt=\"%.2f\"\xff\xff\xff",VPP[4]);
    printf("t11.txt=\"%.2f%%\"\xff\xff\xff",THD*100);
}







// 插值函数
void Interpolate_array(void) {
    const int NUM_INPUT_POINTS = (int)(Fs / Freq[0]+1);
    float buffer[NUM_INPUT_POINTS] ;
    memset(buffer, 0, sizeof(buffer));

    for (int i = 0; i < NUM_INPUT_POINTS; i++) 
    {
        for(int j= 0;j<6;j++)
        {
            buffer[i] += ADC_Buffer[i+j*NUM_INPUT_POINTS];
        }
      buffer[i] = buffer[i]/6;

    }

    // 初始化插值实例
    arm_linear_interp_instance_f32 S;

    // 填充插值实例结构体
    S.nValues = NUM_INPUT_POINTS; // 数据点数量
    S.x1 = 0.0f;                  // 第一个x坐标
    S.xSpacing = 1.0f;            // x轴步长
    S.pYData = buffer;            // y数据

    // 计算每个插值点的x坐标
    float32_t xSpacingOutput = (float32_t)(NUM_INPUT_POINTS - 1)/(NUM_OUTPUT_POINTS - 1); // 输出x轴步长

    for (int i = 0; i < NUM_OUTPUT_POINTS; i++) 
    {
        float32_t x = i * xSpacingOutput;            // 当前插值点的x坐标
        bufferout[i] = arm_linear_interp_f32(&S, x); // 进行线性插值并存储结果
    }
}
