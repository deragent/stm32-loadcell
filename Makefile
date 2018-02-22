# PROJECT DEFINITIONS
CPU 						= cortex-m3
CHIP						= STM32F207xE

EXECUTABLE      = main

SOURCES         = $(wildcard src/*.c) \
		$(wildcard src/*.s) \
		$(wildcard FreeRTOS/port/*.c) \
		$(wildcard FreeRTOS/*.c)

OBJECTS         = $(patsubst %,build/%,$(patsubst %,%.o,$(SOURCES)))

INCLUDE         = -Iinclude \
		-IFreeRTOS/port \
		-IFreeRTOS/include \
    -I../STM32Cube_FW_F2_V1.4.0/Drivers/CMSIS/Device/ST/STM32F2xx/Include \
    -I../STM32Cube_FW_F2_V1.4.0/Drivers/CMSIS/Include

LD              = -T../STM32Cube_FW_F2_V1.4.0/Projects/STM32F207ZG-Nucleo/Templates/SW4STM32/STM32F207xG_Nucleo/STM32F207ZGTx_FLASH.ld


# PIPELINE DEFINITIONS
COMPILE_CMD			= arm-none-eabi-gcc
OBJ_COPY_CMD		= arm-none-eabi-objcopy

COMPILE_ARG			= -Wall -mthumb -Os -std=c99 -g
LINK_ARG 				= -mlittle-endian -mthumb -Wl,--gc-sections -g


COMP						= $(COMPILE_CMD) $(COMPILE_ARG) -mcpu=$(CPU) -D$(CHIP) $(INCLUDE) -c
LINK						= $(COMPILE_CMD) $(LINK_ARG) -mcpu=$(CPU) -D$(CHIP) $(LD)
OBJ_COPY 				= $(OBJ_COPY_CMD) -Oihex


# Make all
.SECONDARY:

all: build_dir $(EXECUTABLE).hex

build_dir:
	mkdir -p $(dir $(OBJECTS))

clean:
	rm -r build/
	rm -f $(EXECUTABLE).elf
	rm -f $(EXECUTABLE).hex

# Compile
build/%.c.o: %.c
	$(COMP) -o $@ $<

build/%.s.o: %.s
	$(COMP) -o $@ $<


# Link
%.elf: $(OBJECTS)
	$(LINK) $(OBJECTS) -o $(EXECUTABLE).elf

# Object Copy
%.hex: %.elf
	$(OBJ_COPY_CMD) $^ $@

# arm-none-eabi-gcc -Wall -mcpu=cortex-m3 -mlittle-endian -mthumb -I../STM32Cube_FW_F2_V1.4.0/Drivers/CMSIS/Device/ST/STM32F2xx/Include -I../STM32Cube_FW_F2_V1.4.0/Drivers/CMSIS/Include -DSTM32F401xE -Os -c main.c -o main.o
# arm-none-eabi-gcc -mcpu=cortex-m3 -mlittle-endian -mthumb -DSTM32F207xE -T../STM32Cube_FW_F2_V1.4.0/Projects/STM32F207ZG-Nucleo/Templates/SW4STM32/STM32F207xG_Nucleo/STM32F207ZGTx_FLASH.ld -Wl,--gc-sections system.o main.o startup_stm32f207xx.o -o main.elf
# arm-none-eabi-objcopy -Oihex main.elf main.hex

program:
	openocd -d0 -f openocd/st_nucleo_f207zg.cfg -c "init;targets;halt;flash write_image erase $(EXECUTABLE).hex;reset run;shutdown"

debug:
	openocd -d0 -f openocd/st_nucleo_f207zg.cfg
