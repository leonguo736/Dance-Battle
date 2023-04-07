################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../include/intelfpgaup/src/HEX.c \
../include/intelfpgaup/src/KEY.c \
../include/intelfpgaup/src/LEDR.c \
../include/intelfpgaup/src/SW.c \
../include/intelfpgaup/src/accel.c \
../include/intelfpgaup/src/audio.c \
../include/intelfpgaup/src/video.c 

OBJS += \
./include/intelfpgaup/src/HEX.o \
./include/intelfpgaup/src/KEY.o \
./include/intelfpgaup/src/LEDR.o \
./include/intelfpgaup/src/SW.o \
./include/intelfpgaup/src/accel.o \
./include/intelfpgaup/src/audio.o \
./include/intelfpgaup/src/video.o 

C_DEPS += \
./include/intelfpgaup/src/HEX.d \
./include/intelfpgaup/src/KEY.d \
./include/intelfpgaup/src/LEDR.d \
./include/intelfpgaup/src/SW.d \
./include/intelfpgaup/src/accel.d \
./include/intelfpgaup/src/audio.d \
./include/intelfpgaup/src/video.d 


# Each subdirectory must supply rules for building sources it contributes
include/intelfpgaup/src/%.o: ../include/intelfpgaup/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -DGCC -DDEBUG -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\drivers" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\hwlib\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\tools\AudioSamples\resources\results" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\intelfpgaup\include" -O0 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


