################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app/echo_btn.c \
../Core/Src/app/echo_flash_memory.c \
../Core/Src/app/echo_led.c \
../Core/Src/app/echo_retarget.c \
../Core/Src/app/echo_shell.c \
../Core/Src/app/echo_state.c \
../Core/Src/app/echo_stim_fdbk.c \
../Core/Src/app/echo_stim_pulse.c \
../Core/Src/app/echo_stim_setting.c \
../Core/Src/app/echo_sys_common.c \
../Core/Src/app/echo_uart2.c 

OBJS += \
./Core/Src/app/echo_btn.o \
./Core/Src/app/echo_flash_memory.o \
./Core/Src/app/echo_led.o \
./Core/Src/app/echo_retarget.o \
./Core/Src/app/echo_shell.o \
./Core/Src/app/echo_state.o \
./Core/Src/app/echo_stim_fdbk.o \
./Core/Src/app/echo_stim_pulse.o \
./Core/Src/app/echo_stim_setting.o \
./Core/Src/app/echo_sys_common.o \
./Core/Src/app/echo_uart2.o 

C_DEPS += \
./Core/Src/app/echo_btn.d \
./Core/Src/app/echo_flash_memory.d \
./Core/Src/app/echo_led.d \
./Core/Src/app/echo_retarget.d \
./Core/Src/app/echo_shell.d \
./Core/Src/app/echo_state.d \
./Core/Src/app/echo_stim_fdbk.d \
./Core/Src/app/echo_stim_pulse.d \
./Core/Src/app/echo_stim_setting.d \
./Core/Src/app/echo_sys_common.d \
./Core/Src/app/echo_uart2.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/app/%.o Core/Src/app/%.su: ../Core/Src/app/%.c Core/Src/app/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DUSE_HAL_DRIVER -DSTM32L412xx -DLED_GREEN_EN -DECHO_SHELL_ENABLED -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/eidos/STM32CubeIDE/workspace_1.10.1/allears_GEN2/Core/Inc/app" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-app

clean-Core-2f-Src-2f-app:
	-$(RM) ./Core/Src/app/echo_btn.d ./Core/Src/app/echo_btn.o ./Core/Src/app/echo_btn.su ./Core/Src/app/echo_flash_memory.d ./Core/Src/app/echo_flash_memory.o ./Core/Src/app/echo_flash_memory.su ./Core/Src/app/echo_led.d ./Core/Src/app/echo_led.o ./Core/Src/app/echo_led.su ./Core/Src/app/echo_retarget.d ./Core/Src/app/echo_retarget.o ./Core/Src/app/echo_retarget.su ./Core/Src/app/echo_shell.d ./Core/Src/app/echo_shell.o ./Core/Src/app/echo_shell.su ./Core/Src/app/echo_state.d ./Core/Src/app/echo_state.o ./Core/Src/app/echo_state.su ./Core/Src/app/echo_stim_fdbk.d ./Core/Src/app/echo_stim_fdbk.o ./Core/Src/app/echo_stim_fdbk.su ./Core/Src/app/echo_stim_pulse.d ./Core/Src/app/echo_stim_pulse.o ./Core/Src/app/echo_stim_pulse.su ./Core/Src/app/echo_stim_setting.d ./Core/Src/app/echo_stim_setting.o ./Core/Src/app/echo_stim_setting.su ./Core/Src/app/echo_sys_common.d ./Core/Src/app/echo_sys_common.o ./Core/Src/app/echo_sys_common.su ./Core/Src/app/echo_uart2.d ./Core/Src/app/echo_uart2.o ./Core/Src/app/echo_uart2.su

.PHONY: clean-Core-2f-Src-2f-app

