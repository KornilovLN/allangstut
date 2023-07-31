################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/MB/str_mbregisters.c \
../Core/MB/str_mbs.c \
../Core/MB/str_tim.c \
../Core/MB/str_uart.c 

OBJS += \
./Core/MB/str_mbregisters.o \
./Core/MB/str_mbs.o \
./Core/MB/str_tim.o \
./Core/MB/str_uart.o 

C_DEPS += \
./Core/MB/str_mbregisters.d \
./Core/MB/str_mbs.d \
./Core/MB/str_tim.d \
./Core/MB/str_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/MB/%.o: ../Core/MB/%.c Core/MB/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Core/Defines -I../Core/Common -I../Core/MB -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

