################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_base.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_p2uart.c \
../intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_printf.c 

S_UPPER_SRCS += \
../intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_base.S 

OBJS += \
./intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_base.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_p2uart.o \
./intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_printf.o 

C_DEPS += \
./intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_base.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_p2uart.d \
./intel-socfpga-hwlib/armv7a/hwlib/src/utils/alt_printf.d 


# Each subdirectory must supply rules for building sources it contributes
intel-socfpga-hwlib/armv7a/hwlib/src/utils/%.o: ../intel-socfpga-hwlib/armv7a/hwlib/src/utils/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-as.exe  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

intel-socfpga-hwlib/armv7a/hwlib/src/utils/%.o: ../intel-socfpga-hwlib/armv7a/hwlib/src/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


