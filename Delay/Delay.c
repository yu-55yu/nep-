#include "Delay.h"

void Delay_ns(uint8_t t)
{
	do
	{
		;
	} while (--t);
}