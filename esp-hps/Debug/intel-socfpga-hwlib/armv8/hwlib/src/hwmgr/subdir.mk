################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_16550_uart.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_a53_timers.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_cache.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_clock_manager.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_dma.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_dma_program.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_eth_phy_ksz9031.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_ethernet.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_generalpurpose_io.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_i2c.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_interrupt.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_mmu.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_sdmmc.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_spi.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_timers.c \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_watchdog.c 

S_UPPER_SRCS += \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_interrupt_aarch32.S \
../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_interrupt_aarch64.S 

OBJS += \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_16550_uart.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_a53_timers.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_cache.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_clock_manager.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_dma.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_dma_program.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_eth_phy_ksz9031.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_ethernet.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_generalpurpose_io.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_i2c.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_interrupt.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_interrupt_aarch32.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_interrupt_aarch64.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_mmu.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_sdmmc.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_spi.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_timers.o \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_watchdog.o 

C_DEPS += \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_16550_uart.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_a53_timers.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_cache.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_clock_manager.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_dma.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_dma_program.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_eth_phy_ksz9031.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_ethernet.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_generalpurpose_io.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_i2c.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_interrupt.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_mmu.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_sdmmc.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_spi.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_timers.d \
./intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/alt_watchdog.d 


# Each subdirectory must supply rules for building sources it contributes
intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/%.o: ../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include" -I"C:\Users\Haxrox\Documents\DS-5 Workspace\test\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -I"C:\Users\Haxrox\Documents\UBC\CPEN-391\intel-socfpga-hwlib\armv7a\hwlib\include\soc_cv_av" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/%.o: ../intel-socfpga-hwlib/armv8/hwlib/src/hwmgr/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-as.exe  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


