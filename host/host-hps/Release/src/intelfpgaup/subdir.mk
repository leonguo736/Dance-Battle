################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/intelfpgaup/HEX.c \
../src/intelfpgaup/KEY.c \
../src/intelfpgaup/LEDR.c \
../src/intelfpgaup/SW.c \
../src/intelfpgaup/accel.c \
../src/intelfpgaup/audio.c \
../src/intelfpgaup/video.c 

OBJS += \
./src/intelfpgaup/HEX.o \
./src/intelfpgaup/KEY.o \
./src/intelfpgaup/LEDR.o \
./src/intelfpgaup/SW.o \
./src/intelfpgaup/accel.o \
./src/intelfpgaup/audio.o \
./src/intelfpgaup/video.o 

C_DEPS += \
./src/intelfpgaup/HEX.d \
./src/intelfpgaup/KEY.d \
./src/intelfpgaup/LEDR.d \
./src/intelfpgaup/SW.d \
./src/intelfpgaup/accel.d \
./src/intelfpgaup/audio.d \
./src/intelfpgaup/video.d 


# Each subdirectory must supply rules for building sources it contributes
src/intelfpgaup/%.o: ../src/intelfpgaup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\drivers" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\hwlib\include" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\tools\AudioSamples\resources\results" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\host-hps\include\intelfpgaup" -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

