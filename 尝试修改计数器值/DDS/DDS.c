// #include "DDS.h"

// //------------------------------------------------------------------------------


// uint32_t place1;
// uint32_t place2;

// uint32_t ctrl_word1;
// uint32_t ctrl_word2;
// uint32_t phase_word[2]={0};
// uint8_t phaseFlag[2]={1};
// float phase[2]={0.f};

// uint16_t pData1[WAVE_POINT]={0};
// uint16_t pData2[WAVE_POINT]={0};


// //channal 0: DAC_CHANNEL_1
// //channal 1: DAC_CHANNEL_2
// // void phaseChange(float phase,int Channal)//相位改变代码
// // {
// //  	phase_word[Channal]=(uint32_t)(phase/360.0*4294967295);
// //  	if(phaseFlag[Channal]==1)
// //  	{
// //  		phaseFlag[Channal]=0;
// //  		phase[Channal]+=phase_word[Channal];
// //  	}
// // }
// //---------------------------------------------------------------------------------------------------------



// uint16_t DAC_Buffer1[WAVE_POINT];
// uint16_t DAC_Buffer2[WAVE_POINT];

// #define PI 3.14159265358979f
// #define AMP_MAX 4095
// uint16_t triangle_wave[WAVE_POINT];
// uint16_t square_wave[WAVE_POINT];
// uint16_t sine_wave[WAVE_POINT];
// ,Signal Wave[2];

// void Generate_Waves(void){
//     for (int i = 0; i < WAVE_POINT; i++) {
//         // 生成三角波（0 到 4095，再回到 0）
//         if (i < WAVE_POINT / 2) {
//             triangle_wave[i] = (uint16_t)((2.0 * AMP_MAX * i) / WAVE_POINT);
//         } else {
//             triangle_wave[i] = (uint16_t)((2.0 * AMP_MAX * (WAVE_POINT - i)) / WAVE_POINT);
//         }


//         // 生成方波（前半 4095，后半 0）
//         square_wave[i] = (i < WAVE_POINT / 2) ? AMP_MAX : 0;

//         // 生成正弦波（0 到 4095）
//         sine_wave[i] = (uint16_t)((sin((2.0 * PI * i) / WAVE_POINT) + 1) * (AMP_MAX / 2));
//     }
// }

// void DDS_SetWaveform(Signal Waves, int Channel) {
//     uint16_t *waveform = NULL;
//     waveform = Waves.Type == TRIANGLE ? triangle_wave : sine_wave;
// 		float AmpWord = Waves.VPP/3000.0;

//     if (Channel == 0) {
//         for (int i = 0; i < WAVE_POINT; i++) {
//             DAC_Buffer1[i] = waveform[i]*AmpWord;
//         }
//     } else if (Channel == 1) {
//         for (int i = 0; i < WAVE_POINT; i++) {
//             DAC_Buffer2[i] = waveform[i]*AmpWord;
//         }
//     }
// }




// void DDS_SetParameter(Signal wave[]) {
// 	ctrl_word1=Wave[0].Freq*OF;//频率控制字
//     ctrl_word2=Wave[1].Freq*OF;//频率控制字

// 	for(int i=0; i<2; i++)
// 	{
// 		DDS_SetWaveform(Wave[i],i);//设置波形
// 	}
// }