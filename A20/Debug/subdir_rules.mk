################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TIsoftwares/ccstheia140/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"D:/TIprojects/A20" -I"D:/TIprojects/A20/MyFunction" -I"D:/TIprojects/A20/Debug" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/Core/Include" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/DSP/Include" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-49370031: ../2021A.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"D:/TIsoftwares/ccstheia140/ccs/utils/sysconfig_1.20.0/sysconfig_cli.bat" --script "D:/TIprojects/A20/2021A.syscfg" -o "." -s "D:/TIsoftwares/mspm0_sdk_1_30_00_03/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-49370031 ../2021A.syscfg
device.opt: build-49370031
device.cmd.genlibs: build-49370031
ti_msp_dl_config.c: build-49370031
ti_msp_dl_config.h: build-49370031
Event.dot: build-49370031

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TIsoftwares/ccstheia140/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"D:/TIprojects/A20" -I"D:/TIprojects/A20/MyFunction" -I"D:/TIprojects/A20/Debug" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/Core/Include" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/DSP/Include" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TIsoftwares/ccstheia140/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"D:/TIprojects/A20" -I"D:/TIprojects/A20/MyFunction" -I"D:/TIprojects/A20/Debug" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/Core/Include" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source" -I"D:/TIsoftwares/mspm0_sdk_1_30_00_03/source/third_party/CMSIS/DSP/Include" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


