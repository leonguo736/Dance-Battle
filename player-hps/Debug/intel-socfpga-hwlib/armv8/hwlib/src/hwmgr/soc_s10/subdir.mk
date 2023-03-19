################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/alt_reset_manager.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/alt_system_manager.c 

OBJS += \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/alt_reset_manager.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/alt_system_manager.o 

C_DEPS += \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/alt_reset_manager.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/alt_system_manager.d 


# Each subdirectory must supply rules for building sources it contributes
intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/%.o: ../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/soc_s10/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


