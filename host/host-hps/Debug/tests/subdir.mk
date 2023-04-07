################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tests/gamescreen.c 

OBJS += \
./tests/gamescreen.o 

C_DEPS += \
./tests/gamescreen.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\gui\tools\AudioSamples\resources\results" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\gui\intelfpgaup\include" -O0 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


