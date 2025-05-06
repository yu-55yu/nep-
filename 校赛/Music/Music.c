#include "ti_msp_dl_config.h"
#include "MyFunction.h"

extern uint16_t ADC_Buffer2[ADC_LEN];

extern float FFT_Output[FFT_LEN];  
double nengliang[10]={0}; 
int BaseIdx_music;                                    //基波在FFT_Output数组里的索引
uint8_t song = 0;


//检测音频20k
void yinpin(void)
{
    uint8_t cnt5=0;//测评3k，只有5歌曲在3k
    uint8_t cnt6=0;//测评电压，只有6歌曲声音很大
    uint8_t cnt7_1=0;//测评500，只有7歌曲在500
    uint8_t cnt7_2=0;
    uint8_t cnt8=0;//测评，只有7歌曲在3
    for(int i=0;i<ADC_LEN;i++)
    {
        if(ADC_Buffer2[i]>3000){
            cnt6++;
        }
    }
    if(cnt6>3){
        song=6;
    }    
    else
    {
        for(int i=150;i<180;i++)
        {
            if(FFT_Output[i]>15000){
                cnt5++;
            }
        }
        if(cnt5>1)
        {
            song = 5;
        }
       
        else
        {
            for(int i=20;i<30;i++)
            {
                if(FFT_Output[i]>25000)
                {
                    cnt7_1++;
                }
            }
            for(int i=75;i<88;i++)
            {
                if(FFT_Output[i]>5000)
                {
                    cnt7_2++;
                }
            }
            if(cnt7_1>1&&cnt7_2)
            {
                song=7;
            }

            else
            {
                 for(int i=75;i<100;i++)
                {
                    if(FFT_Output[i]>10000)
                    {
                        cnt8++;
                    }
                }
                if(cnt8>1)
                {
                    song=8;
                }
               
            }
        }
        
    }  
}




//检测音频(1-4)320
double calculate_rms(float *array, int array_size, int center_index, int radius) 
{
    double sum = 0.0;
    int num_elements = 2 * radius + 1; // 100个元素
    // 计算平方和
    for (int i = -radius; i <= radius; i++) {
        // 计算索引，超出边界时从另一端循环
        int index = (center_index + i + array_size) % array_size;
        sum += array[index] * array[index];
    }   
    // 计算均方根值的平方（即均方和）
    return sum / num_elements;
}


void yinpin2(void)
{
    float ADC_Buff[FFT_LEN]={0};  
    BaseIdx_music = 0;
    float max_val = 0;
    int STEP=100;
    int start_index=0;
    for (int i = 0; i < FFT_LEN; i++) 
    { 
        float a=3.3*ADC_Buffer2[i]/4096;
        ADC_Buff[i]=(a-1.83)*10; //去掉直流
    }  
    for (int i = 0; i < FFT_LEN; i++) 
    { // 遍历 0 ~ Fs/2 部分
        float b=calculate_rms(ADC_Buff,1024,start_index,50);;    
        if (b > max_val) {
            max_val = b;
            BaseIdx_music = i; // 记录最大电压的索引
        }
        start_index += 1;
    }
    start_index=BaseIdx_music;
    for(int i=0;i<10;i++)
    {
        nengliang[i]=calculate_rms(ADC_Buff,1024,start_index,50);
        start_index += STEP;
    }
    //判断
    if(nengliang[0]>1.80||nengliang[0]>nengliang[9]*3.3)
    {
        song=1;
    }
    else 
    {
        if((nengliang[0]-nengliang[9]<0.4)||nengliang[0]<1.05)
        {
            song=3;
        }
        else 
        {
            if(nengliang[0]>nengliang[2]*2)
            {
                song=4;
            }
            else 
            {
                song=2;
            }
        }
    }
    
}