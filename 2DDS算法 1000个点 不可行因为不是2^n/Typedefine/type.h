#ifndef TYPE_H
#define TYPE_H

#include "main.h"


typedef enum {
    SINE,
    TRIANGLE,
} SignalType;


typedef struct{
    SignalType Type;
    float Freq;
}Signal;

 



#endif 
