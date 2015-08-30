################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/netif/ppp/auth.c \
../src/netif/ppp/chap.c \
../src/netif/ppp/chpms.c \
../src/netif/ppp/fsm.c \
../src/netif/ppp/ipcp.c \
../src/netif/ppp/lcp.c \
../src/netif/ppp/magic.c \
../src/netif/ppp/md5.c \
../src/netif/ppp/pap.c \
../src/netif/ppp/ppp.c \
../src/netif/ppp/ppp_oe.c \
../src/netif/ppp/randm.c \
../src/netif/ppp/vj.c 

OBJS += \
./src/netif/ppp/auth.o \
./src/netif/ppp/chap.o \
./src/netif/ppp/chpms.o \
./src/netif/ppp/fsm.o \
./src/netif/ppp/ipcp.o \
./src/netif/ppp/lcp.o \
./src/netif/ppp/magic.o \
./src/netif/ppp/md5.o \
./src/netif/ppp/pap.o \
./src/netif/ppp/ppp.o \
./src/netif/ppp/ppp_oe.o \
./src/netif/ppp/randm.o \
./src/netif/ppp/vj.o 

C_DEPS += \
./src/netif/ppp/auth.d \
./src/netif/ppp/chap.d \
./src/netif/ppp/chpms.d \
./src/netif/ppp/fsm.d \
./src/netif/ppp/ipcp.d \
./src/netif/ppp/lcp.d \
./src/netif/ppp/magic.d \
./src/netif/ppp/md5.d \
./src/netif/ppp/pap.d \
./src/netif/ppp/ppp.d \
./src/netif/ppp/ppp_oe.d \
./src/netif/ppp/randm.d \
./src/netif/ppp/vj.d 


# Each subdirectory must supply rules for building sources it contributes
src/netif/ppp/%.o: ../src/netif/ppp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_CMSISv2p00_LPC17xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\port" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include\ipv4" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


