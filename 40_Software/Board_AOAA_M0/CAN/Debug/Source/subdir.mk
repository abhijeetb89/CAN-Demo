################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Source/IntQueueTimer.c \
../Source/RegTest.c \
../Source/canpt.c \
../Source/cr_startup_lpc11.c \
../Source/main-blinky.c \
../Source/main-full.c \
../Source/main.c 

OBJS += \
./Source/IntQueueTimer.o \
./Source/RegTest.o \
./Source/canpt.o \
./Source/cr_startup_lpc11.o \
./Source/main-blinky.o \
./Source/main-full.o \
./Source/main.o 

C_DEPS += \
./Source/IntQueueTimer.d \
./Source/RegTest.d \
./Source/canpt.d \
./Source/cr_startup_lpc11.d \
./Source/main-blinky.d \
./Source/main-full.d \
./Source/main.d 


# Each subdirectory must supply rules for building sources it contributes
Source/%.o: ../Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC11xx -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_CMSISv2p00_LPC11xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_MCU\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source\Common_Demo_Tasks\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source\FreeRTOS_Source\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source\FreeRTOS_Source\portable\GCC\ARM_CM0" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/cr_startup_lpc11.o: ../Source/cr_startup_lpc11.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC11xx -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_CMSISv2p00_LPC11xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_MCU\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source\Common_Demo_Tasks\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source\FreeRTOS_Source\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\CAN\Source\FreeRTOS_Source\portable\GCC\ARM_CM0" -O0 -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"Source/cr_startup_lpc11.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


