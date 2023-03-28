################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := clean

# All of the sources participating in the build are defined here
-include sources.mk
-include tests/final/subdir.mk
-include intelfpgaup/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: gui

# Tool invocations
gui: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker 10.3.1 [arm-none-linux-gnueabihf]'
	arm-none-linux-gnueabihf-gcc.exe -static  -pthread -o "gui" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES) $(OBJS) $(C_DEPS) gui 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
