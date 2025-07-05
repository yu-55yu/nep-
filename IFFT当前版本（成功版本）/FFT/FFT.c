#include "FFT.h"
#include "main.h"
#include "stdio.h"
#include "string.h"

float FFT_Mag[FFT_LEN];
float FFT_Data[FFT_LEN * 2];
uint16_t ADC_Buffer[ADC_LEN];

float FFT_Data1[FFT_LEN * 2];
float FFT_Data2[FFT_LEN * 2];
uint16_t DAC_Buffer1[DAC_LEN]={0};
uint16_t DAC_Buffer2[DAC_LEN]={0};

extern Signal Wave[2];

float a[2]={0.87046,1.49017};
float b[2]={0.88389,1.63371};
float y[3]={7.7121,-0.3596, 0.1865};

void FFT_Process(uint16_t* adcbuffer)
{
  for(int i=0;i<FFT_LEN;i++)
    {
      FFT_Data[i * 2] = adcbuffer[i];
      FFT_Data[i * 2 + 1] = 0;                    
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_Data, 0, 1);
    arm_cmplx_mag_f32(FFT_Data, FFT_Mag, FFT_LEN);
}


//传入逆变换数组 实部虚部交替的那个
void IFFT_Process(float *fft_data,uint16_t *dac_data)
{
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_data, 1, 1);
    for(int i=0; i<FFT_LEN; i++)
    {
        dac_data[i] = (uint16_t)(fft_data[i*2]/65535*4095+2048);
    }
}


float GetPhase(uint8_t idx)
{
    float phase = atan2f(FFT_Data[idx* 2 + 1], FFT_Data[idx * 2]);
	return phase;
}



// //把FFT_Data(ADC直接FFT得到的)处理成 单频、特定相位 的FFT_Datai
// void ProcessFFTData(uint8_t i, float phase)
// {
//     int idx = Wave[i].BaseIdx;
//     int other_idx = (i==0)?Wave[1].BaseIdx:Wave[0].BaseIdx;
//     int bw = 2; 
//     float* fft_data = (i == 0) ? FFT_Data1 : FFT_Data2;
//     memset(fft_data, 0, FFT_LEN * 2 * sizeof(float));
    
//     float correction=0;

//     if(i==0)
//     {
//         correction = a[0]*idx + a[1];
//     }
//     else if(i==1)
//     {
//         correction = b[0]*idx + b[1];
//         correction = correction+y[0]+y[1]*other_idx+y[2]*idx;
//     }
//     phase += correction; 
//     phase = phase * PI / 180.0;


//     if(Wave[i].Type == TRIANGLE) {
//         for(int n = 1; idx*n*2 < FFT_LEN; n += 2) { // n=1,3,5,...
//             int harm_idx = idx * n;
//             if(harm_idx-bw >= 0 && (harm_idx+bw)*2 < FFT_LEN) {
//                 for(int k = harm_idx-bw; k <= harm_idx+bw; k++) {
//                     // 原始分量
//                     float real = FFT_Data[2*k];
//                     float imag = FFT_Data[2*k+1];
//                     // 新分量加相位
//                     double real_out, imag_out;
//                     add_phase(real, imag, n*phase, &real_out, &imag_out);
//                     fft_data[2*k] = (float)real_out;
//                     fft_data[2*k+1] = (float)imag_out;
//                     // 负频率分量
//                     int mirror = FFT_LEN - k;
//                     if(mirror >= 0 && mirror < FFT_LEN) {
//                         real = FFT_Data[2*mirror];
//                         imag = FFT_Data[2*mirror+1];
//                         add_phase(real, imag, -n*phase, &real_out, &imag_out);
//                         fft_data[2*mirror] = (float)real_out;
//                         fft_data[2*mirror+1] = (float)imag_out;
//                     }
//                 }
//             }
//         }
//     } else {
//         for(int k = idx-bw; k <= idx+bw; k++) {
//             if(k >= 0 && k < FFT_LEN/2) {
//                 float real = FFT_Data[2*k];
//                 float imag = FFT_Data[2*k+1];
//                 double real_out, imag_out;
//                 add_phase(real, imag, phase, &real_out, &imag_out);
//                 fft_data[2*k] = (float)real_out;
//                 fft_data[2*k+1] = (float)imag_out;
//                 int mirror = FFT_LEN - k;
//                 if(mirror >= 0 && mirror < FFT_LEN) {
//                     real = FFT_Data[2*mirror];
//                     imag = FFT_Data[2*mirror+1];
//                     add_phase(real, imag, -phase, &real_out, &imag_out);
//                     fft_data[2*mirror] = (float)real_out;
//                     fft_data[2*mirror+1] = (float)imag_out;
//                 }
//             }
//         }
//     }
// }

// void ProcessFFTData(uint8_t i, float phase)
// {
//     int idx = Wave[i].BaseIdx;
//     int other_idx = (i==0)?Wave[1].BaseIdx:Wave[0].BaseIdx;
//     int bw = 2; 
//     float* fft_data = (i == 0) ? FFT_Data1 : FFT_Data2;
//     memset(fft_data, 0, FFT_LEN * 2 * sizeof(float));
    
//     float correction=0;

//     if(i==0)
//     {
//         correction = a[0]*idx + a[1];
//     }
//     else if(i==1)
//     {
//         correction = b[0]*idx + b[1];
//         correction = correction+y[0]+y[1]*other_idx+y[2]*idx;
//     }
//     phase += correction; 
//     phase = phase * PI / 180.0;

//     if(Wave[i].Type == TRIANGLE) {
//         // 计算基波幅值和相位
//         float A_base = sqrtf(FFT_Data[2*idx]*FFT_Data[2*idx] + FFT_Data[2*idx+1]*FFT_Data[2*idx+1]);
//         float phi_base = atan2f(FFT_Data[2*idx+1], FFT_Data[2*idx]);
//         for(int n = 1; idx*n*2 < FFT_LEN; n += 2) { // n=1,3,5,...
//             int harm_idx = idx * n;
//             if(harm_idx-bw >= 0 && (harm_idx+bw)*2 < FFT_LEN) {
//                 // 检查谐波是否与另一个信号基波重合
//                 int is_overlap = (harm_idx == other_idx);
//                 for(int k = harm_idx-bw; k <= harm_idx+bw; k++) {
//                     if(is_overlap && k == harm_idx) {
//                         // 用公式计算重合分量
//                         float A_harm = A_base / (n*n);
//                         float phi_harm = n * phi_base + PI;
//                         float real = A_harm * cosf(phi_harm);
//                         float imag = A_harm * sinf(phi_harm);
//                         double real_out, imag_out;
//                         add_phase(real, imag, n*phase, &real_out, &imag_out);
//                         fft_data[2*k] = (float)real_out;
//                         fft_data[2*k+1] = (float)imag_out;
//                         // 负频率分量
//                         int mirror = FFT_LEN - k;
//                         if(mirror >= 0 && mirror < FFT_LEN) {
//                             add_phase(real, -imag, -n*phase, &real_out, &imag_out);
//                             fft_data[2*mirror] = (float)real_out;
//                             fft_data[2*mirror+1] = (float)imag_out;
//                         }
//                         continue;
//                     }
//                     // 原始分量
//                     float real = FFT_Data[2*k];
//                     float imag = FFT_Data[2*k+1];
//                     double real_out, imag_out;
//                     add_phase(real, imag, n*phase, &real_out, &imag_out);
//                     fft_data[2*k] = (float)real_out;
//                     fft_data[2*k+1] = (float)imag_out;
//                     // 负频率分量
//                     int mirror = FFT_LEN - k;
//                     if(mirror >= 0 && mirror < FFT_LEN) {
//                         real = FFT_Data[2*mirror];
//                         imag = FFT_Data[2*mirror+1];
//                         add_phase(real, imag, -n*phase, &real_out, &imag_out);
//                         fft_data[2*mirror] = (float)real_out;
//                         fft_data[2*mirror+1] = (float)imag_out;
//                     }
//                 }
//             }
//         }
//     } else {
//         for(int k = idx-bw; k <= idx+bw; k++) {
//             if(k >= 0 && k < FFT_LEN/2) {
//                 float real = FFT_Data[2*k];
//                 float imag = FFT_Data[2*k+1];
//                 double real_out, imag_out;
//                 add_phase(real, imag, phase, &real_out, &imag_out);
//                 fft_data[2*k] = (float)real_out;
//                 fft_data[2*k+1] = (float)imag_out;
//                 int mirror = FFT_LEN - k;
//                 if(mirror >= 0 && mirror < FFT_LEN) {
//                     real = FFT_Data[2*mirror];
//                     imag = FFT_Data[2*mirror+1];
//                     add_phase(real, imag, -phase, &real_out, &imag_out);
//                     fft_data[2*mirror] = (float)real_out;
//                     fft_data[2*mirror+1] = (float)imag_out;
//                 }
//             }
//         }
//     }
// } 
void ProcessFFTData(uint8_t i, float phase)
{
    int idx = Wave[i].BaseIdx;
    int other_idx = (i==0)?Wave[1].BaseIdx:Wave[0].BaseIdx;
    int bw = 2; 
    float* fft_data = (i == 0) ? FFT_Data1 : FFT_Data2;
    memset(fft_data, 0, FFT_LEN * 2 * sizeof(float));
    
    float correction=0;

    if(i==0)
    {
        correction = a[0]*idx + a[1];
    }
    else if(i==1)
    {
        correction = b[0]*idx + b[1];
        correction = correction+y[0]+y[1]*other_idx+y[2]*idx;
    }
    phase += correction; 
    phase = phase * PI / 180.0;

    if(Wave[i].Type == TRIANGLE) {
        // 计算基波幅值和相位
        float A_base = sqrtf(FFT_Data[2*idx]*FFT_Data[2*idx] + FFT_Data[2*idx+1]*FFT_Data[2*idx+1]);
        float phi_base = atan2f(FFT_Data[2*idx+1], FFT_Data[2*idx]);
        for(int n = 1; idx*n*2 < FFT_LEN; n += 2) { // n=1,3,5,...
            int harm_idx = idx * n;
            if(harm_idx-bw >= 0 && (harm_idx+bw)*2 < FFT_LEN) {
                int is_overlap = (harm_idx == other_idx);
                if (Wave[0].Type == TRIANGLE && Wave[1].Type == TRIANGLE) {
                    int other_harm = other_idx;
                    for (int m = 1; other_idx * m * 2 < FFT_LEN; m += 2) {
                        if (harm_idx == other_idx * m) {
                            is_overlap = 1;
                            break;
                        }
                    }
                }
                if(is_overlap) {
                    int k = harm_idx;
                    // 用公式计算重合分量
                    float A_harm = A_base / (n*n);
                    float phi_harm = n * phi_base;
                    float real = A_harm * cosf(phi_harm);
                    float imag = A_harm * sinf(phi_harm);
                    double real_out, imag_out;
                    add_phase(real, imag, n*phase, &real_out, &imag_out);
                    fft_data[2*k] = (float)real_out;
                    fft_data[2*k+1] = (float)imag_out;
                    // 负频率分量
                    int mirror = FFT_LEN - k;
                    if(mirror >= 0 && mirror < FFT_LEN) {
                        add_phase(real, -imag, -n*phase, &real_out, &imag_out);
                        fft_data[2*mirror] = (float)real_out;
                        fft_data[2*mirror+1] = (float)imag_out;
                    }
                    continue;
                }
                for(int k = harm_idx-bw; k <= harm_idx+bw; k++) {
                    // 原始分量
                    float real = FFT_Data[2*k];
                    float imag = FFT_Data[2*k+1];
                    double real_out, imag_out;
                    add_phase(real, imag, n*phase, &real_out, &imag_out);
                    fft_data[2*k] = (float)real_out;
                    fft_data[2*k+1] = (float)imag_out;
                    // 负频率分量
                    int mirror = FFT_LEN - k;
                    if(mirror >= 0 && mirror < FFT_LEN) {
                        real = FFT_Data[2*mirror];
                        imag = FFT_Data[2*mirror+1];
                        add_phase(real, imag, -n*phase, &real_out, &imag_out);
                        fft_data[2*mirror] = (float)real_out;
                        fft_data[2*mirror+1] = (float)imag_out;
                    }
                }
            }
        }
    } else {
        for(int k = idx-bw; k <= idx+bw; k++) {
            if(k >= 0 && k < FFT_LEN/2) {
                float real = FFT_Data[2*k];
                float imag = FFT_Data[2*k+1];
                double real_out, imag_out;
                add_phase(real, imag, phase, &real_out, &imag_out);
                fft_data[2*k] = (float)real_out;
                fft_data[2*k+1] = (float)imag_out;
                int mirror = FFT_LEN - k;
                if(mirror >= 0 && mirror < FFT_LEN) {
                    real = FFT_Data[2*mirror];
                    imag = FFT_Data[2*mirror+1];
                    add_phase(real, imag, -phase, &real_out, &imag_out);
                    fft_data[2*mirror] = (float)real_out;
                    fft_data[2*mirror+1] = (float)imag_out;
                }
            }
        }
    }
} 
// void ProcessFFTData(uint8_t i, float phase)
// {
//     int idx = Wave[i].BaseIdx;
//     int other_idx = (i==0)?Wave[1].BaseIdx:Wave[0].BaseIdx;
//     int bw = 2; 
//     float* fft_data = (i == 0) ? FFT_Data1 : FFT_Data2;
//     memset(fft_data, 0, FFT_LEN * 2 * sizeof(float));
    
//     float correction=0;

//     if(i==0)
//     {
//         correction = a[0]*idx + a[1];
//     }
//     else if(i==1)
//     {
//         correction = b[0]*idx + b[1];
//         correction = correction+y[0]+y[1]*other_idx+y[2]*idx;
//     }
//     phase += correction; 
//     phase = phase * PI / 180.0;

//     if(Wave[i].Type == TRIANGLE) {
//         // 计算基波幅值和相位
//         float A_base = sqrtf(FFT_Data[2*idx]*FFT_Data[2*idx] + FFT_Data[2*idx+1]*FFT_Data[2*idx+1]);
//         float phi_base = atan2f(FFT_Data[2*idx+1], FFT_Data[2*idx]);
//         for(int n = 1; idx*n*2 < FFT_LEN; n += 2) { // n=1,3,5,...
//             int harm_idx = idx * n;
//             if(harm_idx-bw >= 0 && (harm_idx+bw)*2 < FFT_LEN) {
//                 int is_overlap = (harm_idx == other_idx);
//                 if(is_overlap) {
//                     int k = harm_idx;
//                     // 用公式计算重合分量
//                     float A_harm = A_base / (n*n);
//                     float phi_harm = n * phi_base ;
//                     float real = A_harm * cosf(phi_harm);
//                     float imag = A_harm * sinf(phi_harm);
//                     double real_out, imag_out;
//                     add_phase(real, imag, n*phase, &real_out, &imag_out);
//                     fft_data[2*k] = (float)real_out;
//                     fft_data[2*k+1] = (float)imag_out;
//                     // 负频率分量
//                     int mirror = FFT_LEN - k;
//                     if(mirror >= 0 && mirror < FFT_LEN) {
//                         add_phase(real, -imag, -n*phase, &real_out, &imag_out);
//                         fft_data[2*mirror] = (float)real_out;
//                         fft_data[2*mirror+1] = (float)imag_out;
//                     }
//                     continue;
//                 }
//                 for(int k = harm_idx-bw; k <= harm_idx+bw; k++) {
//                     // 原始分量
//                     float real = FFT_Data[2*k];
//                     float imag = FFT_Data[2*k+1];
//                     double real_out, imag_out;
//                     add_phase(real, imag, n*phase, &real_out, &imag_out);
//                     fft_data[2*k] = (float)real_out;
//                     fft_data[2*k+1] = (float)imag_out;
//                     // 负频率分量
//                     int mirror = FFT_LEN - k;
//                     if(mirror >= 0 && mirror < FFT_LEN) {
//                         real = FFT_Data[2*mirror];
//                         imag = FFT_Data[2*mirror+1];
//                         add_phase(real, imag, -n*phase, &real_out, &imag_out);
//                         fft_data[2*mirror] = (float)real_out;
//                         fft_data[2*mirror+1] = (float)imag_out;
//                     }
//                 }
//             }
//         }
//     } else {
//         for(int k = idx-bw; k <= idx+bw; k++) {
//             if(k >= 0 && k < FFT_LEN/2) {
//                 float real = FFT_Data[2*k];
//                 float imag = FFT_Data[2*k+1];
//                 double real_out, imag_out;
//                 add_phase(real, imag, phase, &real_out, &imag_out);
//                 fft_data[2*k] = (float)real_out;
//                 fft_data[2*k+1] = (float)imag_out;
//                 int mirror = FFT_LEN - k;
//                 if(mirror >= 0 && mirror < FFT_LEN) {
//                     real = FFT_Data[2*mirror];
//                     imag = FFT_Data[2*mirror+1];
//                     add_phase(real, imag, -phase, &real_out, &imag_out);
//                     fft_data[2*mirror] = (float)real_out;
//                     fft_data[2*mirror+1] = (float)imag_out;
//                 }
//             }
//         }
//     }
// } 
void Find_2BaseIndex(void)
{
    float max1 = 0, max2 = 0;
    int idx1 = 0, idx2 = 0;

    for (int i = 3; i < FFT_LEN / 2; i++) {
        if (FFT_Mag[i] > max1) {
            max1 = FFT_Mag[i];
            idx1 = i;
        }
    }

    for (int i = 3; i < FFT_LEN / 2; i++) {
        if (i >= idx1 - 2 && i <= idx1 + 2) continue;
        if (FFT_Mag[i] > max2) {
            max2 = FFT_Mag[i];
            idx2 = i;
        }
    }
    // 保证idx小的为Wave[0]
    if (idx1 < idx2) {
        Wave[0].BaseIdx = idx1;
        Wave[1].BaseIdx = idx2;
    } else {
        Wave[0].BaseIdx = idx2;
        Wave[1].BaseIdx = idx1;
    }
}



void Detect_WaveType(void)
{
    for(int i =0;i<2;i++)
    {
        int baseidx = Wave[i].BaseIdx;
        float base = FFT_Mag[baseidx];
        int third_idx = baseidx * 3;
        int fifth_idx = baseidx * 5;
        float third = 0.0f, fifth = 0.0f;
        if (third_idx < FFT_LEN / 2) third = FFT_Mag[third_idx];
        if (fifth_idx < FFT_LEN / 2) fifth = FFT_Mag[fifth_idx];

        int other_idx = (Wave[0].BaseIdx == baseidx) ? Wave[1].BaseIdx: Wave[0].BaseIdx;

        if (other_idx == baseidx * 3 || baseidx == other_idx * 3) {
            float ratio5 = (base > 1e-6f) ? (fifth / base) : 0.0f;
            if (ratio5 > 0.03f && ratio5 < 0.13f) { // 放宽阈值
                Wave[i].Type = TRIANGLE;
            } else {
                Wave[i].Type = SINE;
            }
        } else {
            float ratio3 = (base > 1e-6f) ? (third / base) : 0.0f;
            if (ratio3 > 0.06f && ratio3 < 0.18f) { // 放宽阈值
                Wave[i].Type=TRIANGLE;
            } else {
                Wave[i].Type=SINE;
            }
        }
    }
   
}



/**
 * @brief 给一个复数添加一个相位
 * * @param real_in       输入的原始实部
 * @param imag_in       输入的原始虚部
 * @param delta_theta   要添加的相位（以弧度为单位）
 * @param real_out      用于存储新实部的指针
 * @param imag_out      用于存储新虚部的指针
 */
void add_phase(double real_in, double imag_in, double delta_theta, double* real_out, double* imag_out) {
    // 计算 cos(Δθ) 和 sin(Δθ)
    double cos_delta = cos(delta_theta);
    double sin_delta = sin(delta_theta);

    // 根据推导出的公式计算新的实部和虚部
    // 新实部 = x*cos(Δθ) - y*sin(Δθ)
    // 新虚部 = x*sin(Δθ) + y*cos(Δθ)
    *real_out = real_in * cos_delta - imag_in * sin_delta;
    *imag_out = real_in * sin_delta + imag_in * cos_delta;
}