################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/Sensores/Ultrasonido-HC_SR04/subdir.mk
-include src/Modulos/13-dac/subdir.mk
-include src/Modulos/12-USART/subdir.mk
-include src/Modulos/10-adc/subdir.mk
-include src/Modulos/09-teclado/subdir.mk
-include src/Modulos/08-display_de_segmentos/09C-display/subdir.mk
-include src/Modulos/08-display_de_segmentos/09B-segmentos/subdir.mk
-include src/Modulos/08-display_de_segmentos/09A-barrido/subdir.mk
-include src/Modulos/07-InterrupcionesExternas/subdir.mk
-include src/Modulos/06-PerifericosTemporizados/subdir.mk
-include src/Modulos/05-SalidasDigitales/subdir.mk
-include src/Modulos/04-EntradasDigitales/subdir.mk
-include src/Modulos/03-Timer/subdir.mk
-include src/Modulos/02-GPIO/subdir.mk
-include src/Drivers/Systick/subdir.mk
-include src/Drivers/SwitchMatrix/subdir.mk
-include src/Drivers/IOCON/subdir.mk
-include src/Drivers/C-Timer/subdir.mk
-include src/Drivers/subdir.mk
-include src/Aplicacion/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Mars_Rover-LPC845
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Mars_Rover-LPC845.axf

# Tool invocations
Mars_Rover-LPC845.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU C++ Linker'
	arm-none-eabi-c++ -nostdlib -Xlinker -Map="Mars_Rover-LPC845.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T Mars_Rover-LPC845_Debug.ld -o "Mars_Rover-LPC845.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Mars_Rover-LPC845.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Mars_Rover-LPC845.axf"; # arm-none-eabi-objcopy -v -O binary "Mars_Rover-LPC845.axf" "Mars_Rover-LPC845.bin" ; # checksum -p LPC845 -d "Mars_Rover-LPC845.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
