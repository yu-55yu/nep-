#ifndef TYPE_H
#define TYPE_H

#include "main.h"


typedef enum {
    SINE,
    TRIANGLE,
} SignalType;


typedef struct{
    SignalType Type;
    uint16_t BaseIdx;
    float Phase;
}Signal;


#endif 
