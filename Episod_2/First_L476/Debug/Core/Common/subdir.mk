################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Common/Leds.c \
../Core/Common/str_fifo.c \
../Core/Common/str_flash.c \
../Core/Common/str_mathcommon.c \
../Core/Common/str_service.c \
../Core/Common/str_systick.c 

OBJS += \
./Core/Common/Leds.o \
./Core/Common/str_fifo.o \
./Core/Common/str_flash.o \
./Core/Common/str_mathcommon.o \
./Core/Common/str_service.o \
./Core/Common/str_systick.o 

C_DEPS += \
./Core/Common/Leds.d \
./Core/Common/str_fifo.d \
./Core/Common/str_flash.d \
./Core/Common/str_mathcommon.d \
./Core/Common/str_service.d \
./Core/Common/str_systick.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Common/%.o: ../Core/Common/%.c Core/Common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Core/Defines -I../Core/Common -I../Core/MB -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

