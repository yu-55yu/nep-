#include "MyAD9910.h"
#include <stdint.h>
#include <string.h>


void AD9910_Wave_Set(uint8_t WaveType,float freq,int Duty,float amp);
//void AD9910_WriteCFR3_PLL_SoftSPI(uint8_t N);
void Set_DDS_CLK(uint32_t frequency);
uint32_t parse_serial_data(const uint8_t *data);
void ProcessData(void);
void Set_StepRate(void);
void AD9910_Wave_Sweep(void);