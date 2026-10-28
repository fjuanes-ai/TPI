################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Drivers/SwitchMatrix/swm.cpp 

CPP_DEPS += \
./src/Drivers/SwitchMatrix/swm.d 

OBJS += \
./src/Drivers/SwitchMatrix/swm.o 


# Each subdirectory must supply rules for building sources it contributes
src/Drivers/SwitchMatrix/%.o: ../src/Drivers/SwitchMatrix/%.cpp src/Drivers/SwitchMatrix/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -DCPP_USE_HEAP -D__LPC84X__ -I"/home/iyopolo/Google Drive (local)/ivan.yopolo.3ro/Tareas/UTN/01-Ingenieria_Electronica/Y_02/Informatica_02-I2/TPO-Mars_Rover/git-repo/LPC845_code/Mars_Rover-LPC845/inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-Drivers-2f-SwitchMatrix

clean-src-2f-Drivers-2f-SwitchMatrix:
	-$(RM) ./src/Drivers/SwitchMatrix/swm.d ./src/Drivers/SwitchMatrix/swm.o

.PHONY: clean-src-2f-Drivers-2f-SwitchMatrix

