################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/board.c \
../src/btn.c \
../src/can_handler.c \
../src/light.c \
../src/lm75a.c \
../src/rgb.c 

OBJS += \
./src/board.o \
./src/btn.o \
./src/can_handler.o \
./src/light.o \
./src/lm75a.o \
./src/rgb.o 

C_DEPS += \
./src/board.d \
./src/btn.d \
./src/can_handler.d \
./src/light.d \
./src/lm75a.d \
./src/rgb.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_CMSISv2p00_LPC11xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M0\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


