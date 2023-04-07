################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/esp.c \
../src/regs.c \
../src/uart.c 

OBJS += \
./src/esp.o \
./src/regs.o \
./src/uart.o 

C_DEPS += \
./src/esp.d \
./src/regs.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\drivers" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\hwlib\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\tools\AudioSamples\resources\results" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\intelfpgaup" -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


