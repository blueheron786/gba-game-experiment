# Enhanced GBA C++ Makefile
PROJECT := hello_world
BUILD := build
SOURCES := src
INCLUDES := src

# Toolchain
PREFIX := arm-none-eabi-
CXX := $(PREFIX)g++
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
GBAFIX := gbafix

# Find source files
CPPFILES := $(shell find $(SOURCES) -name "*.cpp")
CFILES := $(shell find $(SOURCES) -name "*.c")

# Object files
OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o)
OFILES := $(addprefix $(BUILD)/, $(notdir $(OFILES)))

# Compiler flags
CXXFLAGS := -mthumb -mthumb-interwork -mcpu=arm7tdmi -mtune=arm7tdmi
CXXFLAGS += -Wall -Wextra -std=c++17 -O2 -fno-exceptions -fno-rtti
CXXFLAGS += -ffunction-sections -fdata-sections
CXXFLAGS += $(foreach dir,$(INCLUDES),-I$(dir))

# Linker flags
LDFLAGS := -mthumb -mthumb-interwork -Wl,--gc-sections -specs=nosys.specs

# Build targets
.PHONY: all clean run debug

all: $(PROJECT).gba

$(BUILD):
	@mkdir -p $(BUILD)

$(PROJECT).elf: $(BUILD) $(OFILES)
	$(CXX) $(LDFLAGS) -o $@ $(OFILES)
	$(OBJDUMP) -h $@ > $(PROJECT).map

$(PROJECT).gba: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
	@if command -v $(GBAFIX) >/dev/null 2>&1; then \
		$(GBAFIX) $@; \
		echo "Fixed GBA header with gbafix"; \
	else \
		echo "Warning: gbafix not found, ROM may not run on real hardware"; \
	fi

$(BUILD)/%.o: $(SOURCES)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SOURCES)/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) *.elf *.gba *.map

run: $(PROJECT).gba
	mgba-qt $<

debug: $(PROJECT).elf
	arm-none-eabi-gdb $<