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
                    float phi_harm = n * phi_base + PI;
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