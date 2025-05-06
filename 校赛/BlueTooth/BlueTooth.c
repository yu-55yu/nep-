#include "BlueTooth.h"
#define ADC_LEN 1024
union Data_send_ hex;//鍙戦€佸寘
uint8_t str[5];//鎺ユ敹鏁扮粍
bool Frame_Head_sta;
bool Frame_End_sta;
extern float VPP;
extern uint16_t ADC_Buffer[ADC_LEN];
extern float Fs;
extern uint16_t MaxVal,MinVal;
extern double Freq;


#define PI 3.14159265358979f
#define WAVE_POINT 25 

int16_t triangle_wave[WAVE_POINT];
int16_t square_wave[WAVE_POINT];
int16_t sine_wave[WAVE_POINT];
extern uint8_t WaveType;//1方波 2三角 3正弦


#pragma pack(1)    //瀛楄妭瀵归綈
typedef struct
{
	char head;
	short int Y_T_3;
	short int Y_T_4;
    short int Y_T_5;
	short int Y_T_6;
    int Y_T_7;
	char check;
	char end;
}Frame;
#pragma pack()

union Data_send_
{
	Frame Frame_data;
	uint8_t str_send[15];
};



void Generate_Waves(int AMP_MAX)
{
    for (int i = 0; i < WAVE_POINT; i++) {
       if (i < WAVE_POINT / 2) {
            triangle_wave[i] = (int16_t)((4.0 * AMP_MAX * i) / WAVE_POINT - AMP_MAX);
        } else {
            triangle_wave[i] = (int16_t)((-4.0 * AMP_MAX * i) / WAVE_POINT + 3 * AMP_MAX);
        }
        square_wave[i] = (i < WAVE_POINT / 2) ? AMP_MAX : -AMP_MAX;

        sine_wave[i] = (int16_t)(sin((2.0 * PI * i) / WAVE_POINT) * AMP_MAX);
    }
}




void Send_Frame(short int vertical,short int x,short int y,short int amp,int horizon)
{
    hex.Frame_data.head = 0xA5;
    hex.Frame_data.Y_T_3 = vertical;
    hex.Frame_data.Y_T_4 = x;
    hex.Frame_data.Y_T_5 = y;
    hex.Frame_data.Y_T_6 = amp;
    hex.Frame_data.Y_T_7 = horizon;
    hex.Frame_data.end = 0x5A;

    int sum = 0;
    for (int i = 1; i < 13; i++)
    {
        sum += hex.str_send[i];
    }
    hex.Frame_data.check = sum % 256;

    for (int i = 0; i < 15; i++)
    {
        while (DL_UART_Main_isBusy(UART_BT_INST));
        DL_UART_Main_transmitData(UART_BT_INST, hex.str_send[i]);
    }
}


void Show_Waveforms(void)
{
    short int x,y,vertical,amp;
    int horizon = (int)(1000000/Freq+0.5);
    amp = (short int)(VPP*500+0.5);
    int AMP_MAX=2500;
    if(amp<40)
    {
        vertical = 50;
    }
    else if(amp<80)
    {
        vertical = 100;
    }
    else if(amp<200)
    {
        vertical = 250;
    }
    else if(amp<400)
    {
        vertical = 500;
    }
    else if(amp<800)
    {
        vertical = 1000;
    }
    else if(amp<1200)
    {
        vertical = 1500;
    }


    AMP_MAX = amp*2500/vertical;
    Generate_Waves(AMP_MAX);



    int16_t*Wavearr = NULL;
    if(WaveType==1)Wavearr=&square_wave;
    if(WaveType==2)Wavearr=&triangle_wave;
    if(WaveType==3)Wavearr=&sine_wave;

    for(int i=0;i<51;i++)
    {
        x=2500/25.0*(i-25);
        y=Wavearr[i%25];

        Send_Frame(vertical,x,y,amp,horizon);
        delay_cycles(10*CPUCLK_FREQ / 1000);
    }
}
