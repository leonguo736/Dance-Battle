################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main.c 

OBJS += \
./main.o 

C_DEPS += \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\kerry\Repos\l2b-34-elur\fpga-hps\hwlib\include\soc_cv_av" -I"C:\Users\kerry\Repos\l2b-34-elur\fpga-hps\hwlib\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


