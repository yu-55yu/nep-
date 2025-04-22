#include "MyFunction.h"
#include <stdio.h>
#define FREQ_MIN 23000000UL
#define FREQ_MAX 25750000UL

uint32_t SYSCLK=1000000000;
extern u32 RAM_POL_SQUARE_WAVE[];
extern u32 RAM_POL_SINE_WAVE[];
u32 SQUARE_WAVE_AMP[2]={0x7fffffff,0x0000ffff};
extern uint8_t rx_buffer[];
extern uint8_t WAVETYPE ;
extern float FREQ ;
extern int DUTY ;
extern float AMP ;
extern uint8_t SquareStepRate[3];
extern uint8_t SineStepRate[3];
extern uint8_t IRQflag;

//------------------中间函数 别管-----------------------------------------------------------------------------------------------------

//设置SI5351 匹配频率
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

//解析按了数字键盘发送到串口的数据包
uint32_t parse_serial_data(const uint8_t *data)
{
  uint8_t data_part[4];
  memcpy(data_part, data + 2, 4);
  uint32_t original_number = (data_part[3] << 24)|(data_part[2] << 16) | (data_part[1] << 8) | data_part[0];
  
  return original_number;
}

//-------------------------------------------------------------------------------------------------------------------------------------



void Set_StepRate(void)
{
  if(rx_buffer[1] == 0x00)
  {
    SquareStepRate[0] = parse_serial_data(rx_buffer);  // 从数据包中解析频率
  }
  else if(rx_buffer[1] == 0x01)
  {
    SquareStepRate[1] = parse_serial_data(rx_buffer);  // 从数据包中解析幅度
  }
  else if(rx_buffer[1] == 0x02)
  {
    SquareStepRate[2] = parse_serial_data(rx_buffer);  // 从数据包中解析占空比
  }
  else if(rx_buffer[1] == 0x03)
  {
    SineStepRate[0] = parse_serial_data(rx_buffer);  // 从数据包中解析频率
  }
  else if(rx_buffer[1] == 0x04)
  {
    SineStepRate[1] = parse_serial_data(rx_buffer);  // 从数据包中解析幅度
  }
  else if(rx_buffer[1] == 0x05)
  {
    SineStepRate[2] = parse_serial_data(rx_buffer);  // 从数据包中解析占空比
  }
}


//设置AD9910的波形类型、频率、幅度(0到16383)和占空比
void AD9910_Wave_Set(uint8_t WaveType,float freq,int Duty,float amp)
{
	if(WaveType == 1)
	{
		printf("page2.t6.txt=\"%dHz\"\xff\xff\xff",(int)freq);
		printf("page2.t8.txt=\"%d%%\"\xff\xff\xff",(int)Duty);
		printf("page2.t7.txt=\"%dmV\"\xff\xff\xff",(int)amp);
	}
	else
	{
		printf("page1.t6.txt=\"%dHz\"\xff\xff\xff",(int)freq);
		printf("page1.t7.txt=\"%dmV\"\xff\xff\xff",(int)amp);
	}
	
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
  uint16_t AmpWord = (uint32_t)(amp*22.8175);
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
				Write_32bit(0x00000003 | (AmpWord<<2));
			}
			else
			{
				Write_32bit(0x7fff0003| (AmpWord<<2));
			}
		}
		else
		{
			uint32_t high16 = srcWaveDta[i] & 0xFFFF0000;
			uint16_t raw14 = (srcWaveDta[i] & 0xFFFC) >> 2; // 提取前14位（舍弃低2位）
			uint16_t scaled14 = (uint16_t)(raw14 * (AmpWord / 16383.0)); // 缩放
			uint16_t low16 = (scaled14 << 2) | (srcWaveDta[i] & 0x0003); // 恢复低两位

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







/// @brief  AD9910波形扫频、扫幅度和扫占空比
/// @param WaveType 波形类型 0:正弦波 1:方波
/// @param FreqStart 频率起始值
/// @param FreqEnd 频率结束值
/// @param FreqStep 频率步进值
/// @param AmpStart 
/// @param AmpEnd 
/// @param AmpStep 
/// @param DutyStart 
/// @param DutyEnd 
/// @param DutyStep 
/// @param duration 
void AD9910_Wave_Sweep(void)
{
  const u8 *StepRate;
  if(rx_buffer[1]<6)
  {
    WAVETYPE = 1;
    StepRate = SquareStepRate;
  }
  else if(rx_buffer[1]>=6)
  {
    WAVETYPE = 0;
    StepRate = SineStepRate;
  }

  float FreqMin = 20;
  float FreqMax = 20000;
  float FreqStart ;
  float FreqEnd ;
	int Direction;
	
	
  if(rx_buffer[1]%2 == 0)
  {
		Direction = 1;
    FreqStart = FreqMin;
    FreqEnd = FreqMax;
  }
  else
  {
		Direction = -1;
    FreqStart =FreqMax;
    FreqEnd = FreqMin;
  }
  
  if(rx_buffer[1]%6 == 0||rx_buffer[1]%6 == 1)
  {
		IRQflag = 1;
    FREQ = FreqStart; // 频率步进 
    while (FREQ <= FreqMax && FREQ >= FreqMin && IRQflag == 1) 
    {
      // 设置方波的频率、幅度和占空比
      AD9910_Init(); // 初始化AD9910
      AD9910_Wave_Set(WAVETYPE, FREQ, DUTY, AMP);
      // 延时一段时间
      HAL_Delay(100); 
      FREQ += (Direction * StepRate[0]); // 更新频率

      // // 如果频率已经超过最大值，则重置频率并更新幅度和占空比
      // FREQ = FreqMax>=FREQ ? FreqMin : FREQ;
      // FREQ = FreqMin<=FREQ ? FreqMax : FREQ;
    }
  }

  if (rx_buffer[1]%6==2||rx_buffer[1]%6==3) 
  {
    AMP+= Direction*StepRate[1]; // 幅度步进
  }
  else if (rx_buffer[1]%6==4||rx_buffer[1]%6==5) 
  {
    DUTY+= Direction*StepRate[2]; // 占空比步进
  }
  AD9910_Init(); // 初始化AD9910
  AD9910_Wave_Set(WAVETYPE, FREQ, DUTY, AMP);

}






/// @brief 修改AD9910的波形参数
void ProcessData(void)
{
    //00~02是方 03~05正弦
    if(rx_buffer[1] < 3) 
    {
      // 方波
      WAVETYPE = 1; // 设置波形类型为方波
    }
    else if(rx_buffer[1] >= 3) 
    {
      // 正弦波
      WAVETYPE = 0; // 设置波形类型为正弦波
    }

    if(rx_buffer[1] %3 == 0) 
    {
      FREQ = parse_serial_data(rx_buffer);  // 从数据包中解析频率
    }
    else if(rx_buffer[1] %3 == 1) 
    {
      AMP = parse_serial_data(rx_buffer);  // 从数据包中解析幅度
    }
    else if(rx_buffer[1] %3 == 2) 
    {
      DUTY = parse_serial_data(rx_buffer);  // 从数据包中解析占空比
    }
      AD9910_Init(); // 初始化AD9910
			AD9910_Wave_Set(WAVETYPE, FREQ, DUTY, AMP);
  
}



