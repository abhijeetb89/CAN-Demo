################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/snmp/asn1_dec.c \
../src/core/snmp/asn1_enc.c \
../src/core/snmp/mib2.c \
../src/core/snmp/mib_structs.c \
../src/core/snmp/msg_in.c \
../src/core/snmp/msg_out.c 

OBJS += \
./src/core/snmp/asn1_dec.o \
./src/core/snmp/asn1_enc.o \
./src/core/snmp/mib2.o \
./src/core/snmp/mib_structs.o \
./src/core/snmp/msg_in.o \
./src/core/snmp/msg_out.o 

C_DEPS += \
./src/core/snmp/asn1_dec.d \
./src/core/snmp/asn1_enc.d \
./src/core/snmp/mib2.d \
./src/core/snmp/mib_structs.d \
./src/core/snmp/msg_in.d \
./src/core/snmp/msg_out.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/snmp/%.o: ../src/core/snmp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_CMSISv2p00_LPC17xx\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_Board\inc" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\port" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_lwip\src\include\ipv4" -I"E:\Special Projects\CAN_HS-LS_GW\40_Software\Board_AOAA_M3\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


