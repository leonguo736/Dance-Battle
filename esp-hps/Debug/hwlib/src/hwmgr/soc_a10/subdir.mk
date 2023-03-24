################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hwlib/src/hwmgr/soc_a10/alt_bridge_manager.c \
../hwlib/src/hwmgr/soc_a10/alt_clock_manager.c \
../hwlib/src/hwmgr/soc_a10/alt_ecc.c \
../hwlib/src/hwmgr/soc_a10/alt_fpga_manager.c \
../hwlib/src/hwmgr/soc_a10/alt_reset_manager.c \
../hwlib/src/hwmgr/soc_a10/alt_sdram.c \
../hwlib/src/hwmgr/soc_a10/alt_system_manager.c 

OBJS += \
./hwlib/src/hwmgr/soc_a10/alt_bridge_manager.o \
./hwlib/src/hwmgr/soc_a10/alt_clock_manager.o \
./hwlib/src/hwmgr/soc_a10/alt_ecc.o \
./hwlib/src/hwmgr/soc_a10/alt_fpga_manager.o \
./hwlib/src/hwmgr/soc_a10/alt_reset_manager.o \
./hwlib/src/hwmgr/soc_a10/alt_sdram.o \
./hwlib/src/hwmgr/soc_a10/alt_system_manager.o 

C_DEPS += \
./hwlib/src/hwmgr/soc_a10/alt_bridge_manager.d \
./hwlib/src/hwmgr/soc_a10/alt_clock_manager.d \
./hwlib/src/hwmgr/soc_a10/alt_ecc.d \
./hwlib/src/hwmgr/soc_a10/alt_fpga_manager.d \
./hwlib/src/hwmgr/soc_a10/alt_reset_manager.d \
./hwlib/src/hwmgr/soc_a10/alt_sdram.d \
./hwlib/src/hwmgr/soc_a10/alt_system_manager.d 


# Each subdirectory must supply rules for building sources it contributes
hwlib/src/hwmgr/soc_a10/%.o: ../hwlib/src/hwmgr/soc_a10/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


