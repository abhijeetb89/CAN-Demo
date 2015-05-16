################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../demo_code/basic_io.c \
../demo_code/consoleprint.c 

OBJS += \
./demo_code/basic_io.o \
./demo_code/consoleprint.o 

C_DEPS += \
./demo_code/basic_io.d \
./demo_code/consoleprint.d 


# Each subdirectory must supply rules for building sources it contributes
demo_code/%.o: ../demo_code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_CMSISv2p00_LPC17xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_FreeRTOS\portable" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_FreeRTOS\include" -O1 -g3 -Wall -c -fmessage-length=0 -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


