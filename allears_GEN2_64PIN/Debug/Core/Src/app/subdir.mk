################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app/echo_adc.c \
../Core/Src/app/echo_btn.c \
../Core/Src/app/echo_flash_memory.c \
../Core/Src/app/echo_led.c \
../Core/Src/app/echo_retarget.c \
../Core/Src/app/echo_shell.c \
../Core/Src/app/echo_stim_pulse.c \
../Core/Src/app/echo_stim_setting.c \
../Core/Src/app/echo_sys_common.c \
../Core/Src/app/echo_sys_fsm_state.c \
../Core/Src/app/echo_uart2.c 

OBJS += \
./Core/Src/app/echo_adc.o \
./Core/Src/app/echo_btn.o \
./Core/Src/app/echo_flash_memory.o \
./Core/Src/app/echo_led.o \
./Core/Src/app/echo_retarget.o \
./Core/Src/app/echo_shell.o \
./Core/Src/app/echo_stim_pulse.o \
./Core/Src/app/echo_stim_setting.o \
./Core/Src/app/echo_sys_common.o \
./Core/Src/app/echo_sys_fsm_state.o \
./Core/Src/app/echo_uart2.o 

C_DEPS += \
./Core/Src/app/echo_adc.d \
./Core/Src/app/echo_btn.d \
./Core/Src/app/echo_flash_memory.d \
./Core/Src/app/echo_led.d \
./Core/Src/app/echo_retarget.d \
./Core/Src/app/echo_shell.d \
./Core/Src/app/echo_stim_pulse.d \
./Core/Src/app/echo_stim_setting.d \
./Core/Src/app/echo_sys_common.d \
./Core/Src/app/echo_sys_fsm_state.d \
./Core/Src/app/echo_uart2.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/app/%.o Core/Src/app/%.su: ../Core/Src/app/%.c Core/Src/app/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L412xx -DDEBUGx -DECHO_SHELL_ENABLED -DLED_GREEN_EN -DECHO_PULSE_INTERRUPT -DECHO_PULSE_DMA -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/eidos/GitHub/allears_GEN2_DEMO/allears_GEN2_64PIN/Core/Inc/app" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-app

clean-Core-2f-Src-2f-app:
	-$(RM) ./Core/Src/app/echo_adc.d ./Core/Src/app/echo_adc.o ./Core/Src/app/echo_adc.su ./Core/Src/app/echo_btn.d ./Core/Src/app/echo_btn.o ./Core/Src/app/echo_btn.su ./Core/Src/app/echo_flash_memory.d ./Core/Src/app/echo_flash_memory.o ./Core/Src/app/echo_flash_memory.su ./Core/Src/app/echo_led.d ./Core/Src/app/echo_led.o ./Core/Src/app/echo_led.su ./Core/Src/app/echo_retarget.d ./Core/Src/app/echo_retarget.o ./Core/Src/app/echo_retarget.su ./Core/Src/app/echo_shell.d ./Core/Src/app/echo_shell.o ./Core/Src/app/echo_shell.su ./Core/Src/app/echo_stim_pulse.d ./Core/Src/app/echo_stim_pulse.o ./Core/Src/app/echo_stim_pulse.su ./Core/Src/app/echo_stim_setting.d ./Core/Src/app/echo_stim_setting.o ./Core/Src/app/echo_stim_setting.su ./Core/Src/app/echo_sys_common.d ./Core/Src/app/echo_sys_common.o ./Core/Src/app/echo_sys_common.su ./Core/Src/app/echo_sys_fsm_state.d ./Core/Src/app/echo_sys_fsm_state.o ./Core/Src/app/echo_sys_fsm_state.su ./Core/Src/app/echo_uart2.d ./Core/Src/app/echo_uart2.o ./Core/Src/app/echo_uart2.su

.PHONY: clean-Core-2f-Src-2f-app

