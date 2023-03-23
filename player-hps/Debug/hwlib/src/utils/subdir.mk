################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hwlib/src/utils/alt_base.c \
../hwlib/src/utils/alt_p2uart.c \
../hwlib/src/utils/alt_printf.c 

S_UPPER_SRCS += \
../hwlib/src/utils/alt_base.S 

OBJS += \
./hwlib/src/utils/alt_base.o \
./hwlib/src/utils/alt_p2uart.o \
./hwlib/src/utils/alt_printf.o 

C_DEPS += \
./hwlib/src/utils/alt_base.d \
./hwlib/src/utils/alt_p2uart.d \
./hwlib/src/utils/alt_printf.d 


# Each subdirectory must supply rules for building sources it contributes
hwlib/src/utils/%.o: ../hwlib/src/utils/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-as.exe  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

hwlib/src/utils/%.o: ../hwlib/src/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


