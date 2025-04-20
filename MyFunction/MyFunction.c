#include "MyFunction.h"

#define FREQ_MIN 23000000UL
#define FREQ_MAX 25750000UL

uint32_t SYSCLK=1000000000;
extern u32 RAM_POL_SQUARE_WAVE[];
extern u32 RAM_POL_SINE_WAVE[];
u32 SQUARE_WAVE_AMP[2]={0x7fffffff,0x0000ffff};

void Set_DDS_CLK(uint32_t targetFreq)
{
    // 中间值：先乘以 1024 再除以 10，使用 64 位防止溢出
    uint64_t baseFreq = (uint64_t)targetFreq * 1024 / 10;

    // 计算 N 的范围
    uint32_t minN = (FREQ_MIN + baseFreq - 1) / baseFreq; // 向上取整
    uint32_t maxN = FREQ_MAX / baseFreq;

    // 选择最小的合法 N（也可以改成 maxN 取最大频率）
		uint32_t N = minN;

    uint32_t DDS_CLK = baseFreq * N;
		SetFrequencyDiv(DDS_CLK,0);//调用锁相环控制时钟(最低8kHZ)
		SYSCLK = DDS_CLK*40;
}


void AD9910_PWM_Wave_Set(uint8_t WaveType,float freq,int Duty,float amp)
{
	Set_DDS_CLK(freq);
  const u32 *srcWaveDta;
  u8 CFR1[] = {0x60, 0x40, 0x00, 0x00};																	// RAM回放目的：幅度;;开启AD9910反Sinc滤波
  //RAM_Profile0[1](高8位)  与  RAM_Profile0[2](低8位)共16位控制字M，决定了输出波形频率， 频率 = Fsysclk / (4*M) / 输出点数 = 1000000000 / (4*M) / 1024
  u8 RAM_Profile0[] = {0x00, 0x00, 0x01, 0xff, 0xc0, 0x00, 0x00, 0x04};	//	地址步进率0X0001=    1,,从地址0回放到1023,共1024点，输出频率 = 1G / (4* 1) / 1024 = 244K Hz  // 连续循环模式RAM_PROx[7]=0x04
  //u8 RAM_Profile0[] = {0x00, 0x00, 0x0A, 0xff, 0xc0, 0x00, 0x00, 0x04};	//	地址步进率0X000A=   10,,从地址0回放到1023,共1024点，输出频率 = 1G / (4*10) / 1024 =  24K Hz  // 连续循环模式RAM_PROx[7]=0x04
  //u8 RAM_Profile0[] = {0x00, 0xFF, 0xFF, 0xff, 0xc0, 0x00, 0x00, 0x04};	//	地址步进率0XFFFF=65535,,从地址0回放到1023,共1024点，输出频率 = 1G / (4*65535) /1024 =3.7 Hz  // 连续循环模式RAM_PROx[7]=0x04
  AD9910_CSN_Clr;
  Write_8bit(0x00); // 将CFR2写入其地址0x01
  for (int i = 0; i < 4; i++)
  {
    Write_8bit(CFR1[i]);
  }
  AD9910_CSN_Set;
  Delay_ns(10);


//频率---------------------------------------------------------------------
  uint32_t FreqWord = (uint32_t) SYSCLK/(4*1024*freq); // 频率控制字，频率 = Fsysclk / (4*M) / 输出点数 = 1000000000 / (4*M) / 1024;
  RAM_Profile0[1] = (FreqWord >> 8) & 0xFF;
  RAM_Profile0[2] = FreqWord & 0xFF;
//------------------------------------------------------------------
  AD9910_CSN_Clr;
  Write_8bit(0X0E);					//写寄存器RAM_Profile0
  for(int i = 0; i < 8; i++)
  {
    Write_8bit(RAM_Profile0[i]);// 将RAM的起始和终止地址、地址步进率写入相应的寄存器  0x0e
  }
  AD9910_CSN_Clr;


  AD9910_IUP_Clr; // 更新AD9910
  Delay_ns(10);
  AD9910_IUP_Set;
  Delay_ns(10);
  AD9910_IUP_Clr;

//幅度---------------------------------------------------------------------
  uint16_t AmpWord = (uint32_t)(amp);
	int DutyWord = (int)Duty/100.0*1024;
//------------------------------------------------------------------

  AD9910_CSN_Clr;
  Write_8bit(0x16);
  switch((uint8_t)WaveType)		//将数据定位到wave控制的数据数组
  {
    case 0:			srcWaveDta = RAM_POL_SINE_WAVE;        break;
    case 1:		  srcWaveDta = SQUARE_WAVE_AMP;      break;
  }
	
  for(int i = 0; i < 1024; i++)
  {
    if(WaveType == 1)
		{
			if(i<DutyWord)
			{
				Write_32bit(0x7fff0000 | AmpWord);
			}
			else
			{
				Write_32bit(0x00000000 | AmpWord);
			}
		}
		else
		{
			uint32_t high16 = srcWaveDta[i] & 0xFFFF0000;
			uint16_t low16 = (srcWaveDta[i] & 0x0000FFFF)*(AmpWord/65535.0);
			Write_32bit(high16 | low16);
		}
     
  }

  AD9910_CSN_Set;
  AD9910_CSN_Clr;
  Write_8bit(0x00);																																// 将CFR1写入其地址0x00
  CFR1[0] |= 0X80; //使能RAM
  for(int i = 0; i < 4; i++)
  Write_8bit(CFR1[i]);
  AD9910_CSN_Set;

  AD9910_IUP_Clr; // 更新AD9910
  Delay_ns(10);
  AD9910_IUP_Set;
  Delay_ns(10);
  AD9910_IUP_Clr;
}
