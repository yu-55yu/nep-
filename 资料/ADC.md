# 定时器触发ADC+DMA


# 1. ADC 配置
## basic configuration
### Sampling Mode Configuration
1. `Conversion Mode` Single 
2. `Enable Repeat Mode`
3. `Sampling Mode` Auto
4. `Trigger Source` Event
#### ADC Conversion Memory 0 Configuration
optional confioguration
- `Trigger Mode` Valid trigger will step to next


## Advance Config
1. `Power Down Mode` Manual
2. `Desired Sample Time 0` 62.5ns

## Interrupt Config
`Enable Interrupts` DMA done


## DMA Config
1. `Enable DMA Trigger`
2. `DMA Samples Count` 1
3. `Enable DMA Triggers	`
MEM0 result loaded interrupt

### DMA Channel
1. `Address Mode` Fixed to Block
2. `Source Length` , `Destination Length` Half Word
3. `Configure Transfer Size`
    1. `Transfer Size` 1000
    2. `Transfer Mode` Repeat Single

## Event Config
> 配完定时器配
`Event Subscriber Channel ID` 1-TIMER_0 => ADC12_0

# 2. TIM 配置
## Basic Configuration
1. `Timer Mode` Periodic Down Counting
    > 数到0 再数到0
2. `Desired Time Period` 1us
    > 采样时间
3. `Start Timer`

## Advanced Config
## Interrupts Config
> 不用产生中断 只用产生事件

## Interrupts Config
1. `Event 1 Publisher Channel ID` 1-Timer_0
    1. `Event 1 Enable Controller Interrupts` Zero event
