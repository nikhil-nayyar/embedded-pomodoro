
PROJECT=embedoro

SRCS := $(shell ls src/)
SRCS_DIR := $(SRCS:%=src/%)
OBJS = $(subst .c,.o, $(SRCS))
OBJS_DIR := $(OBJS:%=obj/%)

LINKER_FILE = ld/TM4C123GH6PM.ld

INC = inc/
CC = arm-none-eabi-gcc -I $(INC) -g
CFLAGS = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16

LD = arm-none-eabi-ld
LDFLAGS = -T$(LINKER_FILE) -e Reset_Handler $(OBJS_DIR) 

OBJCOPY = arm-none-eabi-objcopy 
FLASHER = lm4flash
RM      = rm -rf
MKDIR   = mkdir -p  

all: $(PROJECT).bin

%.bin: %.elf
	$(OBJCOPY) -O binary bin/$^  bin/$(patsubst %.elf, %.bin, $@)

%.elf: $(OBJS)
	mkdir -p bin/
	$(LD) $(LDFLAGS) -o bin/$(PROJECT).elf	

%.o: src/%.c
	mkdir -p obj/
	$(CC) $(CFLAGS) -c $^ -o obj/$(@)

clean:
	rm -rf obj/
	rm -rf bin/

flash: all
	lm4flash bin/$(PROJECT).bin

debug: flash
	openocd -f /home/nayyar/Development/embedded/tools/openocd-code/tcl/board/ti_ek-tm4c123gxl.cfg

test:
	echo $(SRCS)
	echo $(SRCS_DIR)
	echo $(OBJS)
