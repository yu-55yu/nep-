#ifndef TYPE_H
#define TYPE_H

#include "main.h"


typedef enum {
    SINE,
    TRIANGLE,
} SignalType;


typedef struct{
    SignalType Type; 
    float VPP; 
    float Freq; 
    int FreqIndex;
    float Phase;
    float PhasePast;
}Signal;


#endif 
