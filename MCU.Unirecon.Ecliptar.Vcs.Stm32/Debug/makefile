################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/System/Serial/subdir.mk
-include Core/Src/System/Data/subdir.mk
-include Core/Src/System/subdir.mk
-include Core/Src/RC/subdir.mk
-include Core/Src/Drive/subdir.mk
-include Core/Src/Control/subdir.mk
-include Core/Src/Config/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := MCU.Unirecon.Ecliptar.Vcs
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
MCU.Unirecon.Ecliptar.Vcs.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
MCU.Unirecon.Ecliptar.Vcs.list \

OBJCOPY_BIN += \
MCU.Unirecon.Ecliptar.Vcs.bin \


# All Target
all: main-build

# Main-build Target
main-build: MCU.Unirecon.Ecliptar.Vcs.elf secondary-outputs

# Tool invocations
MCU.Unirecon.Ecliptar.Vcs.elf: $(OBJS) $(USER_OBJS) E:\_DEVELING\_PROJECTS\MCU\Unirecon\Ecliptar\Research\EcliptarSTM\MCU.Unirecon.Ecliptar.Vcs\STM32F446RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "MCU.Unirecon.Ecliptar.Vcs.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\_DEVELING\_PROJECTS\MCU\Unirecon\Ecliptar\Research\EcliptarSTM\MCU.Unirecon.Ecliptar.Vcs\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="MCU.Unirecon.Ecliptar.Vcs.map" -Wl,--gc-sections -static --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -u _scanf_float -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

MCU.Unirecon.Ecliptar.Vcs.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "MCU.Unirecon.Ecliptar.Vcs.list"
	@echo 'Finished building: $@'
	@echo ' '

MCU.Unirecon.Ecliptar.Vcs.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "MCU.Unirecon.Ecliptar.Vcs.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
