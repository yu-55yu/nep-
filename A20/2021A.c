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
#include <stdio.h>

extern uint16_t ADC_Buffer[ADC_LEN];
extern volatile uint8_t ChangeSamp;
extern uint16_t arr;

void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_cycles(CPUCLK_FREQ/1000);
    }
}


int fputc(int ch, FILE *f) {
  while (DL_UART_Main_isBusy(UART_0_INST))
    ;
  DL_UART_Main_transmitData(UART_0_INST, ch);
  return ch;
}

int fputs(const char *_ptr, register FILE *_fp) {
  unsigned int len;
  len = strlen(_ptr);
  for (unsigned int i = 0; i < len; i++) {
    while (DL_UART_Main_isBusy(UART_0_INST))
      ;
    DL_UART_Main_transmitData(UART_0_INST, (uint8_t)_ptr[i]);
  }
  return len;
}

int puts(const char *_ptr) {
  int count = fputs(_ptr, stdout);
  count += fputs("\n", stdout);
  return count;
}




int main(void)
{
    SYSCFG_DL_init();

    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)0X40556280);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)ADC_Buffer);
    DL_DMA_enableChannel(DMA,DMA_CH0_CHAN_ID);

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);

    while (1) {

    }
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            // 停止定时器（防止DMA覆盖）
            DL_TimerA_stopCounter(TIMER_0_INST);

            // 做FFT分析
            FFT_Process();
            Find_BaseIndex();

            // 如果是第一次进入，估算频率，设置Fs，让基波处于FFT索引第80位
            if (ChangeSamp == 1)
            {
                Process();
                
                DL_ADC12_clearInterruptStatus(ADC12_0_INST, DL_ADC12_IIDX_DMA_DONE);
                DL_TimerA_setLoadValue(TIMER_0_INST, arr);  // 更新ARR（采样率）
                DL_TimerA_startCounter(TIMER_0_INST);
                break;
            }

            // 第二次进入，重新设采样率，获取一个周期（400点）
            if (ChangeSamp == 2)
            {
                Process();

                DL_ADC12_clearInterruptStatus(ADC12_0_INST, DL_ADC12_IIDX_DMA_DONE);
                DL_TimerA_setLoadValue(TIMER_0_INST, arr);  // 更新采样率
                DL_TimerA_startCounter(TIMER_0_INST);
                break;
            }
                Show_Waveforms();        // 输出波形

            break;

        default:
            break;
    }
}












