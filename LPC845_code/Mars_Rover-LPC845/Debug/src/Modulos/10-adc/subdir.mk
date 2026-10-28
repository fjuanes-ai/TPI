################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Modulos/10-adc/Adc.cpp 

CPP_DEPS += \
./src/Modulos/10-adc/Adc.d 

OBJS += \
./src/Modulos/10-adc/Adc.o 


# Each subdirectory must supply rules for building sources it contributes
src/Modulos/10-adc/%.o: ../src/Modulos/10-adc/%.cpp src/Modulos/10-adc/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -DCPP_USE_HEAP -D__LPC84X__ -I"/home/iyopolo/Google Drive (local)/ivan.yopolo.3ro/Tareas/UTN/01-Ingenieria_Electronica/Y_02/Informatica_02-I2/TPO-Mars_Rover/git-repo/LPC845_code/Mars_Rover-LPC845/inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-Modulos-2f-10-2d-adc

clean-src-2f-Modulos-2f-10-2d-adc:
	-$(RM) ./src/Modulos/10-adc/Adc.d ./src/Modulos/10-adc/Adc.o

.PHONY: clean-src-2f-Modulos-2f-10-2d-adc

