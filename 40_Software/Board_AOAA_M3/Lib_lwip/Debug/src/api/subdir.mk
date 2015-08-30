################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/api/api_lib.c \
../src/api/api_msg.c \
../src/api/err.c \
../src/api/netbuf.c \
../src/api/netdb.c \
../src/api/netifapi.c \
../src/api/sockets.c \
../src/api/tcpip.c 

OBJS += \
./src/api/api_lib.o \
./src/api/api_msg.o \
./src/api/err.o \
./src/api/netbuf.o \
./src/api/netdb.o \
./src/api/netifapi.o \
./src/api/sockets.o \
./src/api/tcpip.o 

C_DEPS += \
./src/api/api_lib.d \
./src/api/api_msg.d \
./src/api/err.d \
./src/api/netbuf.d \
./src/api/netdb.d \
./src/api/netifapi.d \
./src/api/sockets.d \
./src/api/tcpip.d 


# Each subdirectory must supply rules for building sources it contributes
src/api/%.o: ../src/api/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_CMSISv2p00_LPC17xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\port" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include\ipv4" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


