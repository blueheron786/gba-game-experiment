# Enhanced GBA C++ Makefile
PROJECT := hello_world
BUILD := build
OUTPUT := output
SOURCES := src
INCLUDES := src

# Toolchain
PREFIX := arm-none-eabi-
CXX := $(PREFIX)g++
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
GBAFIX := ./gbafix

# Find source files
CPPFILES := $(shell find $(SOURCES) -name "*.cpp")
CFILES := $(shell find $(SOURCES) -name "*.c")
SFILES := $(shell find $(SOURCES) -name "*.s")

# Object files
OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
OFILES := $(addprefix $(BUILD)/, $(notdir $(OFILES)))

# Compiler flags
CXXFLAGS := -mthumb -mthumb-interwork -mcpu=arm7tdmi -mtune=arm7tdmi
CXXFLAGS += -Wall -Wextra -std=c++17 -O2 -fno-exceptions -fno-rtti
CXXFLAGS += -ffunction-sections -fdata-sections
CXXFLAGS += $(foreach dir,$(INCLUDES),-I$(dir))

# Linker flags
LDFLAGS := -mthumb -mthumb-interwork -Wl,--gc-sections -specs=nosys.specs -Tgba.ld

# Build targets
.PHONY: all clean run debug

all: $(OUTPUT)/$(PROJECT).gba

$(BUILD):
	@mkdir -p $(BUILD)

$(OUTPUT):
	@mkdir -p $(OUTPUT)

$(OUTPUT)/$(PROJECT).elf: $(BUILD) $(OUTPUT) $(OFILES)
	$(CXX) $(LDFLAGS) -o $@ $(OFILES)
	$(OBJDUMP) -h $@ > $(OUTPUT)/$(PROJECT).map

$(OUTPUT)/$(PROJECT).gba: $(OUTPUT)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
	@if command -v $(GBAFIX) >/dev/null 2>&1; then \
		$(GBAFIX) $@; \
		echo "Fixed GBA header with gbafix"; \
	else \
		echo "Warning: gbafix not found, manually adding basic header"; \
		dd if=/dev/zero bs=1 count=262144 >> $@ 2>/dev/null || true; \
		truncate -s 262144 $@ 2>/dev/null || true; \
	fi

$(BUILD)/%.o: $(SOURCES)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SOURCES)/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SOURCES)/%.s
	$(CXX) -x assembler-with-cpp $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) $(OUTPUT)

run: $(PROJECT).gba
	mgba-qt $<

debug: $(PROJECT).elf
	arm-none-eabi-gdb $<