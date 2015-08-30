################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Source/FreeRTOS_Source/portable/MemMang/subdir.mk
-include Source/FreeRTOS_Source/portable/GCC/ARM_CM0/subdir.mk
-include Source/FreeRTOS_Source/subdir.mk
-include Source/Common_Demo_Tasks/subdir.mk
-include Source/subdir.mk
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
all: CAN.axf

# Tool invocations
CAN.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_Board\Debug" -L"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_CMSISv2p00_LPC11xx\Debug" -L"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_MCU\Debug" -L"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\linker" -Xlinker -Map=CAN.map -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "RTOSDemo_Debug.ld" -o "CAN.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) CAN.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size CAN.axf; # arm-none-eabi-objcopy -O binary CAN.axf CAN.bin ; checksum -p LPC11C24_301 -d CAN.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
