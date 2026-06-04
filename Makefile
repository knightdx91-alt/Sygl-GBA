.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/gba_rules

TARGET		:= sygl
BUILD		:= build
SOURCES		:= src
INCLUDES	:= src
DATA		:=

ARCH		:= -mthumb -mthumb-interwork

CFLAGS		:= -g -Wall -O2 -mcpu=arm7tdmi -mtune=arm7tdmi \
			   -fomit-frame-pointer -ffast-math $(ARCH)
CFLAGS		+= $(INCLUDE)

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS		:= $(ARCH)
LDFLAGS		:= -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS		:= -lgba
LIBDIRS		:= $(LIBGBA)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:= $(CURDIR)/$(TARGET)
export VPATH	:= $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:= $(CURDIR)/$(BUILD)

CFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export LD	:= $(CC)
export OFILES	:= $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:= $(foreach dir,$(INCLUDES),$(addprefix -iquote ,$(CURDIR)/$(dir))) \
				   $(foreach dir,$(LIBDIRS),$(addprefix -I,$(dir)/include)) \
				   -I$(CURDIR)/$(BUILD)

export LIBPATHS := $(foreach dir,$(LIBDIRS),$(addprefix -L,$(dir)/lib))

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).gba

else

DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).gba : $(OUTPUT).elf
$(OUTPUT).elf : $(OFILES)

-include $(DEPENDS)

endif
