#include "MyAD9910.h"
#include <stdint.h>

void AD9910_PWM_Wave_Set(uint8_t WaveType,float freq,int Duty,float amp);
//void AD9910_WriteCFR3_PLL_SoftSPI(uint8_t N);
void Set_DDS_CLK(uint32_t frequency);