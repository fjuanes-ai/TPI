################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Modulos/02-GPIO/GPIO.cpp 

CPP_DEPS += \
./src/Modulos/02-GPIO/GPIO.d 

OBJS += \
./src/Modulos/02-GPIO/GPIO.o 


# Each subdirectory must supply rules for building sources it contributes
src/Modulos/02-GPIO/%.o: ../src/Modulos/02-GPIO/%.cpp src/Modulos/02-GPIO/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -DCPP_USE_HEAP -D__LPC84X__ -I"C:\Users\franc\Documents\MCUXpressoIDE_25.6.136\workspace\LPC-Cerebro_Auto\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-Modulos-2f-02-2d-GPIO

clean-src-2f-Modulos-2f-02-2d-GPIO:
	-$(RM) ./src/Modulos/02-GPIO/GPIO.d ./src/Modulos/02-GPIO/GPIO.o

.PHONY: clean-src-2f-Modulos-2f-02-2d-GPIO

