#include "si5351.h"
//#include "delay.h"
 uint8_t Div;
 uint8_t Multi;
 
 
 
 
#define SYSCLK 480   //输入10  测试得出是48Khz

void HAL_Delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
    uint32_t reload=SysTick->LOAD;
    ticks=nus*SYSCLK; 
    told=SysTick->VAL; 
    while(1)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)tcnt+=told-tnow;
            else tcnt+=reload-tnow+told;
            told=tnow;
            if(tcnt>=ticks)break; 
        }
    };
}
 
 
void delay_us1(uint16_t n)
{

HAL_Delay_us(n);

}
 
 
 
//void delay_us1(uint16_t n)//约1us,1100k
//{
////	n=n*100;//1.5Mhz
//	
////	n=n*10000;//1.5Mhz
//	
//	
//	
//  unsigned int x=5,i=0;
//  for(i=0;i<n;i++)
//  {while(x--){}x=5;}
//}






void IICstart()//IIC总线起始信号
{
	  SDA(1);
	  CLK(1);
	  SDA(0);
    delay_us1(100); 
	  CLK(0);
    delay_us1(100); 	
}
void IICstop()//IIC总线停止信号
{
	  CLK(0);
    delay_us1(100); 
	  SDA(0);
    delay_us1(100); 
	  CLK(1);
    delay_us1(100); 
	  SDA(1);	
    delay_us1(100); 
}
void IICsend(uint8_t DATA)//IIC总线发送信数据
{
		uint16_t i;
    for(i=0;i<8;i++) //发送一个字节数据
    {     
        CLK(0); 		 //拉低时钟线,准备开始给SDA赋值
			
        if((DATA&0x80)==0)  
        {SDA(0);}  
        else  
        {SDA(1);}  
				
        DATA<<=1;  
        delay_us1(100);    
        CLK(1);
        delay_us1(100);      //等待从设备把数据接收完
		}  
    CLK(0);  
    SDA(1);	 				   //释放数据线
    delay_us1(100);   
    CLK(1); 
    delay_us1(100); 
		while(i<1000){i++;}//等待从设备应答ACK
		CLK(0); 					 //释放时钟线,为下次操作做准备
}
void IICsendreg(uint8_t reg, uint8_t data)
{
  IICstart();      //起始信号
  delay_us1(200);  
  IICsend(0xC0);   //发送设备地址+写信号
  delay_us1(200);  
  IICsend(reg);    //内部寄存器地址
  delay_us1(200);  
  IICsend(data);   //内部寄存器数据
  delay_us1(200);  
  IICstop(); 
}

void SetFrequency(uint32_t frequency,uint8_t div,uint8_t Channel)//设置时钟频率
{
  uint32_t pllFreq;
  uint32_t xtalFreq = XTAL_FREQ;// 晶体频率
  uint32_t l;
  float f;
  uint8_t mult;
  uint32_t num;
  uint32_t denom;
  uint32_t divider;
  divider = 900000000 / frequency;	//锁相环频率:900 mhz
  if (divider % 2) divider--;				//确保一个更整数除法比率
  pllFreq = divider * frequency;		//计算pllFrequency:分频器*所需的输出频率
  mult = pllFreq / xtalFreq;				//确定所需的pllFrequency的乘数
  l = pllFreq % xtalFreq;						//它有三个部分:
  f = l;														//乘是一个整数,必须在15 . . 90
  f *= 1048575;											//num和分母项是小数部分,分子和分母
  f /= xtalFreq;										//每20位(范围0 . . 1048575)
  num = f;													//实际的乘数是乘+ num /分母项
  denom = 1048575;									//为简单起见我们将分母最大1048575
																		//设置锁相环与倍增系数计算
  if(Channel == 0)
  {
	  SetPLLClk(SI_SYNTH_PLL_A, mult, num, denom);


	  SetMultisynth(SI_SYNTH_MS_0,divider,div);
	  IICsendreg(SI_PLL_RESET,0xA0);

	  IICsendreg(SI_CLK0_CONTROL, 0x4F|SI_CLK_SRC_PLL_A);
  }
  if(Channel == 1)
  {
	  SetPLLClk(SI_SYNTH_PLL_B, mult, num, denom);
	  SetMultisynth(SI_SYNTH_MS_1,divider,div);
	  IICsendreg(SI_PLL_RESET,0xA0);
	  IICsendreg(SI_CLK1_CONTROL, 0x4F|SI_CLK_SRC_PLL_B);
  }

}
void SetPLLClk(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom)//设置PPL时钟
{
  uint32_t P1;					// PLL config register P1
  uint32_t P2;					// PLL config register P2
  uint32_t P3;					// PLL config register P3

  P1 = (uint32_t)(128 * ((float)num / (float)denom));
  P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
  P2 = (uint32_t)(128 * ((float)num / (float)denom));
  P2 = (uint32_t)(128 * num - denom * P2);
  P3 = denom;

  IICsendreg(pll + 0, (P3 & 0x0000FF00) >> 8);
  IICsendreg(pll + 1, (P3 & 0x000000FF));
  IICsendreg(pll + 2, (P1 & 0x00030000) >> 16);
  IICsendreg(pll + 3, (P1 & 0x0000FF00) >> 8);
  IICsendreg(pll + 4, (P1 & 0x000000FF));
  IICsendreg(pll + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
  IICsendreg(pll + 6, (P2 & 0x0000FF00) >> 8);
  IICsendreg(pll + 7, (P2 & 0x000000FF));
}
void SetMultisynth(uint8_t synth,uint32_t divider,uint8_t rDiv)//设置多synth
{
  uint32_t P1;					// Synth config register P1
  uint32_t P2;					// Synth config register P2
  uint32_t P3;					// Synth config register P3

  P1 = 128 * divider - 512;
  P2 = 0;							// P2 = 0, P3 = 1 forces an integer value for the divider
  P3 = 1;

  IICsendreg(synth + 0,   (P3 & 0x0000FF00) >> 8);
  IICsendreg(synth + 1,   (P3 & 0x000000FF));
  IICsendreg(synth + 2,   ((P1 & 0x00030000) >> 16) | rDiv);
  IICsendreg(synth + 3,   (P1 & 0x0000FF00) >> 8);
  IICsendreg(synth + 4,   (P1 & 0x000000FF));
  IICsendreg(synth + 5,   ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
  IICsendreg(synth + 6,   (P2 & 0x0000FF00) >> 8);
  IICsendreg(synth + 7,   (P2 & 0x000000FF));
}

void SetFrequencyDiv(uint32_t Frequency,uint8_t Channel)
{
  if(Frequency>1000000){
    Div=SI_R_DIV_1;
    Multi=1;
  }else if(Frequency>500000&&Frequency<=1000000){
    Div=SI_R_DIV_2;
    Multi=2;
  }else if(Frequency>250000&&Frequency<=500000){
    Div=SI_R_DIV_4;
    Multi=4;
  }else if(Frequency>125000&&Frequency<=250000){
    Div=SI_R_DIV_8;
    Multi=8;
  }else if(Frequency>62500&&Frequency<=125000){
    Div=SI_R_DIV_16;
    Multi=16;
  }else if(Frequency>31250&&Frequency<=62500){
    Div=SI_R_DIV_32;
    Multi=32;
  }else if(Frequency>15625&&Frequency<=31250){
    Div=SI_R_DIV_64;
    Multi=64;
  }else if(Frequency>=8&&Frequency<=15625){
    Div=SI_R_DIV_128;
    Multi=128;
  }
  SetFrequency(Frequency*Multi,Div,Channel);
}
