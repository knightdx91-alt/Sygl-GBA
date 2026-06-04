#
# Sygl-GBA Makefile — requires devkitARM
# Install: https://devkitpro.org/wiki/Getting_Started
#
ifeq ($(strip $(DEVKITARM)),)
$(error "Set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/gba_rules

TARGET   := sygl
SOURCES  := src
INCLUDES := src
DATA     :=

ARCH     := -mthumb -mthumb-interwork
CFLAGS   := -g -Wall -O2 -ffunction-sections -fdata-sections $(ARCH) $(INCLUDE) -DGBA
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS  := -g $(ARCH)
LDFLAGS  := -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS     := -lmm -lgba

include $(DEVKITARM)/base_rules

$(OUTPUT).gba: $(OUTPUT).elf
	$(OBJCOPY) -O binary $< $@
	$(DEVKITPRO)/tools/bin/gbafix $@
