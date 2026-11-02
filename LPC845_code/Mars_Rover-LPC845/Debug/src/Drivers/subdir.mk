################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Drivers/cr_cpp_config.cpp \
../src/Drivers/cr_startup_lpc84x.cpp 

C_SRCS += \
../src/Drivers/crp.c \
../src/Drivers/mtb.c 

S_SRCS += \
../src/Drivers/aeabi_romdiv_patch.s 

CPP_DEPS += \
./src/Drivers/cr_cpp_config.d \
./src/Drivers/cr_startup_lpc84x.d 

C_DEPS += \
./src/Drivers/crp.d \
./src/Drivers/mtb.d 

OBJS += \
./src/Drivers/aeabi_romdiv_patch.o \
./src/Drivers/cr_cpp_config.o \
./src/Drivers/cr_startup_lpc84x.o \
./src/Drivers/crp.o \
./src/Drivers/mtb.o 


# Each subdirectory must supply rules for building sources it contributes
src/Drivers/%.o: ../src/Drivers/%.s src/Drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -D__NEWLIB__ -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC84X__ -I"C:\Users\franc\Documents\MCUXpressoIDE_25.6.136\workspace\LPC-Cerebro_Auto\inc" -g3 -gdwarf-4 -mcpu=cortex-m0 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Drivers/%.o: ../src/Drivers/%.cpp src/Drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -DCPP_USE_HEAP -D__LPC84X__ -I"C:\Users\franc\Documents\MCUXpressoIDE_25.6.136\workspace\LPC-Cerebro_Auto\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Drivers/%.o: ../src/Drivers/%.c src/Drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -DCPP_USE_HEAP -D__LPC84X__ -I"C:\Users\franc\Documents\MCUXpressoIDE_25.6.136\workspace\LPC-Cerebro_Auto\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-Drivers

clean-src-2f-Drivers:
	-$(RM) ./src/Drivers/aeabi_romdiv_patch.o ./src/Drivers/cr_cpp_config.d ./src/Drivers/cr_cpp_config.o ./src/Drivers/cr_startup_lpc84x.d ./src/Drivers/cr_startup_lpc84x.o ./src/Drivers/crp.d ./src/Drivers/crp.o ./src/Drivers/mtb.d ./src/Drivers/mtb.o

.PHONY: clean-src-2f-Drivers

