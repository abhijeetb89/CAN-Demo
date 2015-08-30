################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/def.c \
../src/core/dhcp.c \
../src/core/dns.c \
../src/core/init.c \
../src/core/mem.c \
../src/core/memp.c \
../src/core/netif.c \
../src/core/pbuf.c \
../src/core/raw.c \
../src/core/stats.c \
../src/core/sys.c \
../src/core/tcp.c \
../src/core/tcp_in.c \
../src/core/tcp_out.c \
../src/core/timers.c \
../src/core/udp.c 

OBJS += \
./src/core/def.o \
./src/core/dhcp.o \
./src/core/dns.o \
./src/core/init.o \
./src/core/mem.o \
./src/core/memp.o \
./src/core/netif.o \
./src/core/pbuf.o \
./src/core/raw.o \
./src/core/stats.o \
./src/core/sys.o \
./src/core/tcp.o \
./src/core/tcp_in.o \
./src/core/tcp_out.o \
./src/core/timers.o \
./src/core/udp.o 

C_DEPS += \
./src/core/def.d \
./src/core/dhcp.d \
./src/core/dns.d \
./src/core/init.d \
./src/core/mem.d \
./src/core/memp.d \
./src/core/netif.d \
./src/core/pbuf.d \
./src/core/raw.d \
./src/core/stats.d \
./src/core/sys.d \
./src/core/tcp.d \
./src/core/tcp_in.d \
./src/core/tcp_out.d \
./src/core/timers.d \
./src/core/udp.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/%.o: ../src/core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_CMSISv2p00_LPC17xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\port" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include\ipv4" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


