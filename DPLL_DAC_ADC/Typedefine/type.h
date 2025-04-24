#ifndef TYPE_H
#define TYPE_H

#include "main.h"


typedef enum {
    SINE,
    TRIANGLE,
} SignalType;


typedef struct{
    SignalType Type; // Signal type (SINE or TRIANGLE)
    float VPP; // Peak-to-Peak Voltage
    float Freq; // Frequency
}Signal;


#endif 
