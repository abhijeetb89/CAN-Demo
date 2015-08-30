################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USB/Core/LPC/DCD/LPC18XX/Endpoint_LPC18xx.c 

OBJS += \
./Drivers/USB/Core/LPC/DCD/LPC18XX/Endpoint_LPC18xx.o 

C_DEPS += \
./Drivers/USB/Core/LPC/DCD/LPC18XX/Endpoint_LPC18xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USB/Core/LPC/DCD/LPC18XX/%.o: ../Drivers/USB/Core/LPC/DCD/LPC18XX/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__LPC17XX__ -DUSB_DEVICE_ONLY -D__CODE_RED -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_CMSISv2p00_LPC17xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=gnu99 -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


