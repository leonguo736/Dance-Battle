################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../hwlib/src/hwmgr/alt_interrupt_armcc.s \
../hwlib/src/hwmgr/alt_interrupt_armclang.s 

C_SRCS += \
../hwlib/src/hwmgr/alt_16550_uart.c \
../hwlib/src/hwmgr/alt_address_space.c \
../hwlib/src/hwmgr/alt_cache.c \
../hwlib/src/hwmgr/alt_can.c \
../hwlib/src/hwmgr/alt_dma.c \
../hwlib/src/hwmgr/alt_dma_program.c \
../hwlib/src/hwmgr/alt_eth_phy_ksz9031.c \
../hwlib/src/hwmgr/alt_ethernet.c \
../hwlib/src/hwmgr/alt_generalpurpose_io.c \
../hwlib/src/hwmgr/alt_globaltmr.c \
../hwlib/src/hwmgr/alt_i2c.c \
../hwlib/src/hwmgr/alt_interrupt.c \
../hwlib/src/hwmgr/alt_mmu.c \
../hwlib/src/hwmgr/alt_nand.c \
../hwlib/src/hwmgr/alt_qspi.c \
../hwlib/src/hwmgr/alt_safec.c \
../hwlib/src/hwmgr/alt_sdmmc.c \
../hwlib/src/hwmgr/alt_spi.c \
../hwlib/src/hwmgr/alt_timers.c \
../hwlib/src/hwmgr/alt_watchdog.c 

OBJS += \
./hwlib/src/hwmgr/alt_16550_uart.o \
./hwlib/src/hwmgr/alt_address_space.o \
./hwlib/src/hwmgr/alt_cache.o \
./hwlib/src/hwmgr/alt_can.o \
./hwlib/src/hwmgr/alt_dma.o \
./hwlib/src/hwmgr/alt_dma_program.o \
./hwlib/src/hwmgr/alt_eth_phy_ksz9031.o \
./hwlib/src/hwmgr/alt_ethernet.o \
./hwlib/src/hwmgr/alt_generalpurpose_io.o \
./hwlib/src/hwmgr/alt_globaltmr.o \
./hwlib/src/hwmgr/alt_i2c.o \
./hwlib/src/hwmgr/alt_interrupt.o \
./hwlib/src/hwmgr/alt_interrupt_armcc.o \
./hwlib/src/hwmgr/alt_interrupt_armclang.o \
./hwlib/src/hwmgr/alt_mmu.o \
./hwlib/src/hwmgr/alt_nand.o \
./hwlib/src/hwmgr/alt_qspi.o \
./hwlib/src/hwmgr/alt_safec.o \
./hwlib/src/hwmgr/alt_sdmmc.o \
./hwlib/src/hwmgr/alt_spi.o \
./hwlib/src/hwmgr/alt_timers.o \
./hwlib/src/hwmgr/alt_watchdog.o 

C_DEPS += \
./hwlib/src/hwmgr/alt_16550_uart.d \
./hwlib/src/hwmgr/alt_address_space.d \
./hwlib/src/hwmgr/alt_cache.d \
./hwlib/src/hwmgr/alt_can.d \
./hwlib/src/hwmgr/alt_dma.d \
./hwlib/src/hwmgr/alt_dma_program.d \
./hwlib/src/hwmgr/alt_eth_phy_ksz9031.d \
./hwlib/src/hwmgr/alt_ethernet.d \
./hwlib/src/hwmgr/alt_generalpurpose_io.d \
./hwlib/src/hwmgr/alt_globaltmr.d \
./hwlib/src/hwmgr/alt_i2c.d \
./hwlib/src/hwmgr/alt_interrupt.d \
./hwlib/src/hwmgr/alt_mmu.d \
./hwlib/src/hwmgr/alt_nand.d \
./hwlib/src/hwmgr/alt_qspi.d \
./hwlib/src/hwmgr/alt_safec.d \
./hwlib/src/hwmgr/alt_sdmmc.d \
./hwlib/src/hwmgr/alt_spi.d \
./hwlib/src/hwmgr/alt_timers.d \
./hwlib/src/hwmgr/alt_watchdog.d 


# Each subdirectory must supply rules for building sources it contributes
hwlib/src/hwmgr/%.o: ../hwlib/src/hwmgr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

hwlib/src/hwmgr/%.o: ../hwlib/src/hwmgr/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-as.exe  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


