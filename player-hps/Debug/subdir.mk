################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../esp.c \
../hex.c \
../main.c \
../uart.c 

OBJS += \
./esp.o \
./hex.o \
./main.o \
./uart.o 

C_DEPS += \
./esp.d \
./hex.d \
./main.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\esp-hps\hwlib\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\esp-hps\intelfpgaup\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\esp-hps\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


