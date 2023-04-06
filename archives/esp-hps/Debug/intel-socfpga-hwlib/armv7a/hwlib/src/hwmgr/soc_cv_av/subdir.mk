################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_armcc.s \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_gnu.s 

C_SRCS += \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_ecc.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_sdram.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_system_manager.c 

OBJS += \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_armcc.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_gnu.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_ecc.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_sdram.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_system_manager.o 

C_DEPS += \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_ecc.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_sdram.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/alt_system_manager.d 


# Each subdirectory must supply rules for building sources it contributes
intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/%.o: ../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-as.exe  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/%.o: ../intel-socfpga-hwlib/armv7a/hwlib/src/hwmgr/soc_cv_av/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


