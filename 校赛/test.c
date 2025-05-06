/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "MyFunction.h"



 //TIM------------------------------------------------------------------------------------
uint32_t NbLoadValue;   
uint16_t GATE_OPEN=0,GATE_READY2CLOSE=0;
uint32_t Nx,Nb;
double Freq;

volatile uint8_t CC1_Flag = 0;




//ADC--------------------------------------------------------------------------------
uint16_t ADC_Buffer[ADC_LEN];
uint64_t arr;
float Fs;
int BaseIdx;    
float VPP;
uint8_t WaveType;
float MaxV,MinV;
uint16_t Period;
volatile uint8_t ADC_ConvEnd = 0;
volatile uint8_t TriangleSamp = 0;

//LCD
volatile uint8_t Rx_Flag = 0;
volatile uint8_t uart_data = 0;


//music
uint16_t ADC_Buffer2[ADC_LEN];
volatile uint8_t ADC_ConvEnd2 = 0;
extern float FFT_Output[FFT_LEN];  
extern uint8_t song;
int cnt=20;
volatile uint8_t test2=1; 

volatile uint8_t automeasure=0;

void disableADC0(void)
{
    DL_ADC12_disableConversions(ADC12_0_INST);                      // 停止 ADC 转换
    DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);              // 停用 DMA 通道 0
    DL_Timer_stopCounter(TIMER_ADC0_INST);                            // 停止定时器计数器
    DL_Timer_setLoadValue(TIMER_ADC0_INST, 0);  
    memset(ADC_Buffer,0,sizeof(ADC_Buffer));
}

void disableADC1(void)
{
    DL_ADC12_disableConversions(ADC12_1_INST);                      // 停止 ADC 转换
    DL_DMA_disableChannel(DMA, DMA_CH1_CHAN_ID);              // 停用 DMA 通道 0
    DL_Timer_stopCounter(TIMER_ADC1_INST);                            // 停止定时器计数器
    DL_Timer_setLoadValue(TIMER_ADC1_INST, 0);  
    memset(ADC_Buffer2,0,sizeof(ADC_Buffer2));
}


void enableADC0(void)
{
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, DL_ADC12_getMemResultAddress(ADC12_0_INST,ADC12_0_ADCMEM_0) );
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)ADC_Buffer);
    DL_ADC12_enableConversions(ADC12_0_INST);   
    DL_TimerA_startCounter(TIMER_ADC0_INST);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
}
void enableADC1(void)
{
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, DL_ADC12_getMemResultAddress(ADC12_1_INST,ADC12_1_ADCMEM_0) );
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)ADC_Buffer2);
    DL_ADC12_enableConversions(ADC12_1_INST);
    DL_TimerA_startCounter(TIMER_ADC1_INST);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);
}


void StartTIM(void)
{ 
    GATE_OPEN=0,GATE_READY2CLOSE=0;
    Nx=0,Nb=0;
    DL_Timer_setTimerCount(TIMER_0_INST, 0);
    DL_Timer_setTimerCount(TIMER_1_INST, 0);
    DL_Timer_setTimerCount(CAPTURE_0_INST, 0);
    DL_TimerG_startCounter(CAPTURE_0_INST);
}

void StopTIM(void)
{
    DL_Timer_stopCounter(TIMER_1_INST);
    DL_Timer_stopCounter(TIMER_0_INST);
    DL_Timer_stopCounter(CAPTURE_0_INST);
}

int main(void)  
{
    SYSCFG_DL_init();

//输入捕获定时器-----------------------------------------------------------------------------------------------------
    DL_TimerG_setCoreHaltBehavior(CAPTURE_0_INST, DL_TIMER_CORE_HALT_IMMEDIATE);
    NVIC_EnableIRQ(CAPTURE_0_INST_INT_IRQN);
    DL_TimerG_startCounter(CAPTURE_0_INST);



//ADC-----------------------------------------------------------------------------------------------------------
    disableADC1();
    disableADC0();


//UART----------------------------------------------------------------
	NVIC_ClearPendingIRQ(UART_LCD_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_LCD_INST_INT_IRQN);


    while (1) {

        //测频率、设置采样率 开启ADC
        if(CC1_Flag)
        {
            CC1_Flag = 0;
            if (!GATE_OPEN) 
            {  
                GATE_OPEN = 1;    
                DL_TimerA_startCounter(TIMER_0_INST);
                NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
                DL_TimerA_startCounter(TIMER_1_INST);
            }
            else
            {
                Nx++;
            }

            if (GATE_OPEN && GATE_READY2CLOSE)
            {
                DL_Timer_stopCounter(TIMER_1_INST);
                DL_Timer_stopCounter(TIMER_0_INST);
                DL_Timer_stopCounter(CAPTURE_0_INST);

                // 读取 TIM2 计数值 Nb
                Nb = DL_TimerG_getLoadValue(TIMER_0_INST)-DL_TimerG_getTimerCount(TIMER_0_INST);

                // Freq = Nx2 / Nb * 240000000（TIM0 的计数时钟频率）
                Freq = ((double)Nx) / ((double)(Nb/5000000.0*256));
                ChangSamp(Freq, &Fs, &arr);
                Fs = 16000000.0/(arr+1);
               

                DL_TimerA_setLoadValue(TIMER_ADC0_INST, arr);
                 // 更新硬件定时器与DMA
                 disableADC1();
                 enableADC0();

            }
        }

//====ADC中断标志位 分析打印波形 测峰峰值================================================================================================
        if(ADC_ConvEnd)
        {
            ADC_ConvEnd = 0;
//FFT
            window(0);
            FFT_Process(ADC_Buffer);
//找基波
            Find_BaseIndex(&BaseIdx);
            if(Freq<1.8)
            {
                BaseIdx=1;
            }
//分析 判断波形
            analyze_waveform(Fs);



//矫正方波的高频段
            if(WaveType==1&&Freq>=21)
            { 
                int start = 0, end = 0;
                Find_MaxSegment_RangeWithin(ADC_Buffer, 1024, 30, &start, &end);
                float max_sum = 0, min_sum = 0;
                int max_cnt = 0, min_cnt = 0;
                float T = (float)Fs / Freq;         // 一个周期采样点数
                float half_T = T / 2.0f;            // 半周期长度
                float pos = (float)(start + end) / 2.0f;  // 从中段中心开始取点

                for (int i = 0; i < 10; i++) {
                    int mid = (int)(pos + 0.5f);  // 四舍五入到最接近整数

                    if (mid - 1 < 0 || mid + 1 >= 1024) break;  // 防止越界

                    float sum = 0;
                    for (int j = mid - 1; j <= mid + 1; j++) {
                        sum += ADC_Buffer[j];
                    }

                    if (i % 2 == 0) {
                        max_sum += sum;
                        max_cnt += 3;
                    } else {
                        min_sum += sum;
                        min_cnt += 3;
                    }

                    pos += half_T;  // 精确地往后推半周期
                }

                float vmax = max_sum / max_cnt;
                float vmin = min_sum / min_cnt;
                VPP = fabs(vmax - vmin) * 3.3f / 4096.0f;
                VPP = VPP/3.0;//实际电压三倍

            }

//矫方波低频段、正弦波、三角波
            else {
                Find_MaxMin(&MaxV,&MinV);
                VPP = MaxV - MinV;//遍历判断最大最小值
                VPP = VPP/3.0;//实际电压三倍

                if(WaveType==1)//方波
                {
                    VPP = VPP-0.0085;
                    if(VPP<0.07)
                    {
                        VPP=0.06;
                    }
                }
                if(WaveType==3)//正弦
                {
                    VPP = VPP-0.007;
                    if(VPP<0.06f)
                    {
                        VPP=0.06;
                    }
                }
                if(WaveType==2)
                {
                    if(VPP<0.2f)
                    {
                        VPP=VPP-0.005;
                    }
                    if(VPP<0.068f)
                    {
                        VPP=0.0605;
                    }
     
                }
            }


//打印数据
            printf("t3.txt=\"%.2f Hz\"\xff\xff\xff",Freq);
            printf("t4.txt=\"%.2f mV\"\xff\xff\xff",1000*VPP);
            if(WaveType ==1)
            {
                printf("t5.txt=\"SQUARE\"\xff\xff\xff");
            }
            else if(WaveType == 2)
            {
                printf("t5.txt=\"TRIANGLE\"\xff\xff\xff");
            }
             else if(WaveType == 3)
            {
                printf("t5.txt=\"SINE\"\xff\xff\xff");
            }
            Show_Waveforms();

            if(automeasure){
                StartTIM();
            }


        }
//==================================================================================================================

        //串口选择测量信号/音频
        if(Rx_Flag)
        {
            Rx_Flag = 0;

            //重新测量
            if(uart_data == 0x01)
            {
                StopTIM();
                StartTIM();
                automeasure=0;
            }

            //测音频
            if(uart_data==0x02)
            {
                DL_TimerA_setLoadValue(TIMER_ADC1_INST, 99);
                //改变采样率，再开adc；第一次没判断出（标志位song==0），再改采样率开adc
                disableADC0();
                enableADC1();
            }

            if(uart_data==0x03)
            {
                StopTIM();
                StartTIM();
                automeasure=1;
            }


        }
 

        //测音频
        if(ADC_ConvEnd2)
        {
            ADC_ConvEnd2 = 0;

            if ((cnt != 0) && (song == 0)) 
            {
                cnt--;
                FFT_Process(ADC_Buffer2);
                yinpin();
                enableADC1();
            }
            else if(song==0&&test2)
            {
                test2=0;
                DL_TimerA_setLoadValue(TIMER_ADC1_INST, 2000000/320-1);
                disableADC0();
                enableADC1();
            }
            else if(song==0&&test2==0)
            {
                yinpin2();
                disableADC1();
                printf("t6.txt=\"song:%d\"\xff\xff\xff",song);
                song = 0;
                cnt=20;
                test2=1;
            }
            else
            {
               disableADC1();
               printf("t6.txt=\"song:%d\"\xff\xff\xff",song);
               song = 0;
               cnt=20;
               test2=1;
            }
        }
    }
}




void CAPTURE_0_INST_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(CAPTURE_0_INST)) {
        case DL_TIMER_IIDX_CC1_DN:
            CC1_Flag =1;

        default:
            break;
        }
}

void TIMER_1_INST_IRQHandler(void)
{
    switch (DL_TimerA_getPendingInterrupt(TIMER_1_INST)) {
        case DL_TIMERA_IIDX_ZERO:
            GATE_READY2CLOSE=1;
            break;
        default:
            break;
    }
}


//测幅值
void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            // 停止定时器（防止DMA覆盖）
            DL_TimerA_stopCounter(TIMER_ADC0_INST);
            DL_ADC12_clearInterruptStatus(ADC12_0_INST, DL_ADC12_IIDX_DMA_DONE);

            ADC_ConvEnd = 1;
           
            break;

        default:
            break;
    }
}


//接收指令
void UART_LCD_INST_IRQHandler(void) {
  switch (DL_UART_Main_getPendingInterrupt(UART_LCD_INST)) {
    case DL_UART_MAIN_IIDX_RX:
        uart_data = DL_UART_Main_receiveData(UART_LCD_INST);
        Rx_Flag = 1;
    break;
  default:
    break;
  }
}



//测音频
void ADC12_1_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_1_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            // 停止定时器（防止DMA覆盖）
            DL_TimerA_stopCounter(TIMER_ADC1_INST);
            DL_ADC12_clearInterruptStatus(ADC12_1_INST, DL_ADC12_IIDX_DMA_DONE);
            ADC_ConvEnd2 = 1;
            break;

        default:
            break;
    }
}