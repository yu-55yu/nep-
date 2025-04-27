好！既然你要的是**数字锁相环算法（Digital PLL, DPLL）**，那我就给你：

> ✅ **完整的思路解释 + 数学公式 + STM32 实现建议**，让你可以从混合信号中提取出目标频率波形，并使 DAC 输出波形在示波器上相对“锁定”参考波。

------

## 📌 DPLL 原理（针对你这个“未知相位、已知频率”的场景）

虽然你不能直接获取参考信号的相位，但你知道它的频率（f₀），同时你能采到混合信号。数字锁相环要做的就是：

> “调整一个 NCO（数控振荡器）输出波形的相位，使它与混合信号中的参考波形相位一致，从而看起来是锁定的。”

------

## 🧠 DPLL 框图逻辑如下：

```
text复制编辑           +------------------+          +--------------+
  ADC -->  | 相位比较器/乘法器 |  ---->  | 环路滤波器/PI  | --> 调整频率/相位 -->
           +------------------+          +--------------+
                                                   ↓
                                             +-------------+
                                             |    NCO      |
                                             | (查表输出)  |
                                             +-------------+
                                                   ↓
                                               DAC 输出
```

------

## ✅ DPLL 各个部分详解

### 1️⃣ ADC 输入

你采样的混合信号 `C[n] = A[n] + B[n]`，我们假设参考信号是 `A[n] = sin(2πf₀·n/Fs + φ₀)`。

### 2️⃣ 数控振荡器（NCO）

NCO 的核心是一个查表振荡器，用于产生 `sin(θ[n])`：

```
c


复制编辑
theta[n+1] = theta[n] + 2πf₀ / Fs + phase_correction
```

- `theta[n]` 是当前的相位角
- `phase_correction` 是由相位误差计算出的校正项

**θ 是 0~~2π，通常映射为 0~~N 表，查 sin/cos 表输出**

------

### 3️⃣ 相位检测（用混合信号与 NCO 信号相乘）

```
c


复制编辑
error[n] = C[n] * sin(theta[n])   // 相当于锁定目标频率分量的相位误差
```

然后用一个低通滤波器提取误差信号中的 DC 分量（即相位差）：

```
c


复制编辑
e_filtered[n] = α * e_filtered[n-1] + (1 - α) * error[n]
```

α 越接近 1，滤波越强（相当于 IIR 低通）

------

### 4️⃣ 控制器（PI 控制，调节 NCO 相位）

```
c复制编辑delta_phase += Kp * e_filtered[n] + Ki * integral
integral += e_filtered[n]
```

这个 `delta_phase` 会被加到 `theta[n]` 上，用于动态调整 NCO 输出的相位，实现“锁定”。

------

## 📈 整体控制流程简化代码（伪代码）

```
c复制编辑float theta = 0.0f;
float delta_phase = 2 * PI * f0 / Fs;
float phase_correction = 0.0f;
float integral = 0.0f;
float error_filtered = 0.0f;

for (int n = 0; n < N; n++) {
    float c = adc_input[n];
    float ref = sin(theta);
    
    // 相位比较
    float error = c * ref;

    // 简单低通滤波器
    error_filtered = alpha * error_filtered + (1 - alpha) * error;

    // PI 控制器
    integral += error_filtered;
    float correction = Kp * error_filtered + Ki * integral;

    // 更新 NCO 相位
    theta += delta_phase + correction;
    if (theta > 2 * PI) theta -= 2 * PI;

    // DAC 输出锁定波形
    dac_output[n] = sin(theta);
}
```

------

## 💡 参数设置建议



| 参数         | 含义         | 建议初值                         |
| ------------ | ------------ | -------------------------------- |
| `Fs`         | 采样率       | 比参考波高 10~20 倍              |
| `f₀`         | 目标参考频率 | 确定的参考波频率                 |
| `alpha`      | 低通滤波系数 | 0.95 ~ 0.99                      |
| `Kp`, `Ki`   | 控制器参数   | 试验调整，Kp=0.05，Ki=0.005 起步 |
| `NCO` 表长度 | 正弦表大小   | 1024 点或更高，防止抖动          |

------

## 🛠️ 实现建议（STM32 H7）

- **ADC**：配置高速 ADC，采样频率如 1MHz；
- **DAC**：通过 DMA 输出查表结果；
- **中断/定时器**：用定时器定频调用锁相环更新；
- **查表法 NCO**：提前建立 sin 表存储在 RAM；
- **FPU 支持**：H7 内置 FPU，用 float 实现运算效率更高。

------

## ✅ 最终你能达到的效果：

- 从混合波中提取出参考频率成分；
- DAC 输出锁定参考频率波；
- 示波器上参考波和 DAC 输出波“相对静止”，不会漂移；
- 可作为同步信号，帮助触发分离显示。

------



- ADC 采样配置
- DPLL 算法核心
- DAC 输出查表
- 采样调试信号输出等