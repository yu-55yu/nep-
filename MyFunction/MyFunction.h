#include  "main.h"
#include <string.h>

void SPI_Refresh(void);

void SPI_WriteRegister(uint8_t reg,uint8_t regSize, uint8_t *value);
void AD9910_Freq_W(float Freq);
void SetFreq(float Freq);
void Paramter_Init(void);
void Write_Waveform(void);