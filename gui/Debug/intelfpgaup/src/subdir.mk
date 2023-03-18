################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../intelfpgaup/src/HEX.c \
../intelfpgaup/src/KEY.c \
../intelfpgaup/src/LEDR.c \
../intelfpgaup/src/SW.c \
../intelfpgaup/src/accel.c \
../intelfpgaup/src/audio.c \
../intelfpgaup/src/video.c 

OBJS += \
./intelfpgaup/src/HEX.o \
./intelfpgaup/src/KEY.o \
./intelfpgaup/src/LEDR.o \
./intelfpgaup/src/SW.o \
./intelfpgaup/src/accel.o \
./intelfpgaup/src/audio.o \
./intelfpgaup/src/video.o 

C_DEPS += \
./intelfpgaup/src/HEX.d \
./intelfpgaup/src/KEY.d \
./intelfpgaup/src/LEDR.d \
./intelfpgaup/src/SW.d \
./intelfpgaup/src/accel.d \
./intelfpgaup/src/audio.d \
./intelfpgaup/src/video.d 


# Each subdirectory must supply rules for building sources it contributes
intelfpgaup/src/%.o: ../intelfpgaup/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -std=c99 -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\gui\tools\AudioSamples\resources\results" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\l2b-34-elur\gui\intelfpgaup\include" -O0 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


