################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/ipv4/autoip.c \
../src/core/ipv4/icmp.c \
../src/core/ipv4/igmp.c \
../src/core/ipv4/inet.c \
../src/core/ipv4/inet_chksum.c \
../src/core/ipv4/ip.c \
../src/core/ipv4/ip_addr.c \
../src/core/ipv4/ip_frag.c 

OBJS += \
./src/core/ipv4/autoip.o \
./src/core/ipv4/icmp.o \
./src/core/ipv4/igmp.o \
./src/core/ipv4/inet.o \
./src/core/ipv4/inet_chksum.o \
./src/core/ipv4/ip.o \
./src/core/ipv4/ip_addr.o \
./src/core/ipv4/ip_frag.o 

C_DEPS += \
./src/core/ipv4/autoip.d \
./src/core/ipv4/icmp.d \
./src/core/ipv4/igmp.d \
./src/core/ipv4/inet.d \
./src/core/ipv4/inet_chksum.d \
./src/core/ipv4/ip.d \
./src/core/ipv4/ip_addr.d \
./src/core/ipv4/ip_frag.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/ipv4/%.o: ../src/core/ipv4/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_CMSISv2p00_LPC17xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\port" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include\ipv4" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


