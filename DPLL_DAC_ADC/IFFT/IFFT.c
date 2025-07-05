// #include "IFFT.h"

// /* 
// ********************************************************************************************************* 
// * 
// 函 数 名: arm_rfft_f32_app 
// * 
// 功能说明: 调用函数arm_rfft_fast_f32 计算幅频和相频 
// * 
// 形    参：无 
// * 
// 返 回 值: 无 
// ********************************************************************************************************* 
// */ 
// static void arm_rfft_f32_app(void) 
// { 
// uint16_t i; 
// arm_rfft_fast_instance_f32 S; 
// /* 正变换 */                                                            
// ifftFlag = 0;  
  
//  /* 初始化结构体S中的参数 */ 
//   arm_rfft_fast_init_f32(&S, TEST_LENGTH_SAMPLES); 
  
//  for(i=0; i<1024; i++) 
//  { 
//   /* 波形是由直流分量，50Hz正弦波组成，波形采样率1024，初始相位60° */ 
//   testInput_f32[i] = 1 + cos(2*3.1415926f*50*i/1024 + 3.1415926f/3); 
//  } 
  
//  /* 1024点实序列快速FFT */  
//  arm_rfft_fast_f32(&S, testInput_f32, testOutput_f32, ifftFlag); 
  
//  /* 为了方便跟函数arm_cfft_f32计算的结果做对比，这里求解了1024组模值，实际函数arm_rfft_fast_f32 
//     只求解出了512组   
//  */  
//   arm_cmplx_mag_f32(testOutput_f32, testOutputMag_f32, TEST_LENGTH_SAMPLES); 
  
  
//  printf("=========================================\r\n");  
  
//  /* 求相频 */ 
//  PowerPhaseRadians_f32(testOutput_f32, Phase_f32, TEST_LENGTH_SAMPLES, 0.5f); 
  
  
//  /* 串口打印求解的幅频和相频 */ 
//  for(i=0; i<TEST_LENGTH_SAMPLES; i++) 
//  { 
//   printf("%f, %f\r\n", testOutputMag_f32[i], Phase_f32[i]); 
//  } 
// } 
