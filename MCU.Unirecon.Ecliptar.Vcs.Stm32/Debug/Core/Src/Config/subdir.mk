################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Config/adc.c \
../Core/Src/Config/crc.c \
../Core/Src/Config/dma.c \
../Core/Src/Config/gpio.c \
../Core/Src/Config/i2c.c \
../Core/Src/Config/rtc.c \
../Core/Src/Config/spi.c \
../Core/Src/Config/stm32f4xx_hal_msp.c \
../Core/Src/Config/syscalls.c \
../Core/Src/Config/sysmem.c \
../Core/Src/Config/system_stm32f4xx.c \
../Core/Src/Config/tim.c \
../Core/Src/Config/usart.c 

CPP_SRCS += \
../Core/Src/Config/stm32f4xx_it.cpp 

C_DEPS += \
./Core/Src/Config/adc.d \
./Core/Src/Config/crc.d \
./Core/Src/Config/dma.d \
./Core/Src/Config/gpio.d \
./Core/Src/Config/i2c.d \
./Core/Src/Config/rtc.d \
./Core/Src/Config/spi.d \
./Core/Src/Config/stm32f4xx_hal_msp.d \
./Core/Src/Config/syscalls.d \
./Core/Src/Config/sysmem.d \
./Core/Src/Config/system_stm32f4xx.d \
./Core/Src/Config/tim.d \
./Core/Src/Config/usart.d 

OBJS += \
./Core/Src/Config/adc.o \
./Core/Src/Config/crc.o \
./Core/Src/Config/dma.o \
./Core/Src/Config/gpio.o \
./Core/Src/Config/i2c.o \
./Core/Src/Config/rtc.o \
./Core/Src/Config/spi.o \
./Core/Src/Config/stm32f4xx_hal_msp.o \
./Core/Src/Config/stm32f4xx_it.o \
./Core/Src/Config/syscalls.o \
./Core/Src/Config/sysmem.o \
./Core/Src/Config/system_stm32f4xx.o \
./Core/Src/Config/tim.o \
./Core/Src/Config/usart.o 

CPP_DEPS += \
./Core/Src/Config/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Config/adc.o: ../Core/Src/Config/adc.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/adc.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/crc.o: ../Core/Src/Config/crc.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/crc.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/dma.o: ../Core/Src/Config/dma.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/dma.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/gpio.o: ../Core/Src/Config/gpio.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/gpio.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/i2c.o: ../Core/Src/Config/i2c.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/i2c.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/rtc.o: ../Core/Src/Config/rtc.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/rtc.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/spi.o: ../Core/Src/Config/spi.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/spi.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/stm32f4xx_hal_msp.o: ../Core/Src/Config/stm32f4xx_hal_msp.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/stm32f4xx_hal_msp.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/stm32f4xx_it.o: ../Core/Src/Config/stm32f4xx_it.cpp Core/Src/Config/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/stm32f4xx_it.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/syscalls.o: ../Core/Src/Config/syscalls.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/syscalls.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/sysmem.o: ../Core/Src/Config/sysmem.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/sysmem.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/system_stm32f4xx.o: ../Core/Src/Config/system_stm32f4xx.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/system_stm32f4xx.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/tim.o: ../Core/Src/Config/tim.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/tim.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Config/usart.o: ../Core/Src/Config/usart.c Core/Src/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"E:/_DEVELING/_PROJECTS/MCU/Unirecon/Ecliptar/Research/EcliptarSTM/MCU.Unirecon.Ecliptar.Vcs/Core/Inc/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Config/usart.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

