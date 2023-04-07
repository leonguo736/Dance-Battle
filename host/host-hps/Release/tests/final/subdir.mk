################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tests/final/fonts.c \
../tests/final/main.c \
../tests/final/vgatools.c 

OBJS += \
./tests/final/fonts.o \
./tests/final/main.o \
./tests/final/vgatools.o 

C_DEPS += \
./tests/final/fonts.d \
./tests/final/main.d \
./tests/final/vgatools.d 


# Each subdirectory must supply rules for building sources it contributes
tests/final/%.o: ../tests/final/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\drivers" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\hwlib\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\tools\AudioSamples\resources\results" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\intelfpgaup" -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


