################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
BlueTooth/%.o: ../BlueTooth/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TIsoftwares/ccstheia140/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"D:/CCSworkspace/test" -I"D:/CCSworkspace/test/Music" -I"D:/CCSworkspace/test/BlueTooth" -I"D:/CCSworkspace/test/Print" -I"D:/CCSworkspace/test/MyFunction" -I"D:/CCSworkspace/test/Debug" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/Core/Include" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/DSP/Include" -gdwarf-3 -MMD -MP -MF"BlueTooth/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


