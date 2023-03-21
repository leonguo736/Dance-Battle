################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_armcc.s \
../hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_gnu.s 

C_SRCS += \
../hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.c \
../hwlib/src/hwmgr/soc_cv_av/alt_ecc.c \
../hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_sdram.c \
../hwlib/src/hwmgr/soc_cv_av/alt_system_manager.c 

OBJS += \
./hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_armcc.o \
./hwlib/src/hwmgr/soc_cv_av/alt_bridge_f2s_gnu.o \
./hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.o \
./hwlib/src/hwmgr/soc_cv_av/alt_ecc.o \
./hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_sdram.o \
./hwlib/src/hwmgr/soc_cv_av/alt_system_manager.o 

C_DEPS += \
./hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.d \
./hwlib/src/hwmgr/soc_cv_av/alt_ecc.d \
./hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_sdram.d \
./hwlib/src/hwmgr/soc_cv_av/alt_system_manager.d 


# Each subdirectory must supply rules for building sources it contributes
hwlib/src/hwmgr/soc_cv_av/%.o: ../hwlib/src/hwmgr/soc_cv_av/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-as.exe  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

hwlib/src/hwmgr/soc_cv_av/%.o: ../hwlib/src/hwmgr/soc_cv_av/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


