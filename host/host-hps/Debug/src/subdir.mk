################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fonts.c \
../src/main.c \
../src/vgatools.c 

OBJS += \
./src/fonts.o \
./src/main.o \
./src/vgatools.o 

C_DEPS += \
./src/fonts.d \
./src/main.d \
./src/vgatools.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -DGCC -DDEBUG -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\drivers" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\hwlib\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host\host-hps\include\intelfpgaup" -O0 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


