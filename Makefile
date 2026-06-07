PROJ_NAME       := sygl
PROJ_TITLE      := Sygl
PROJ_MAKER      := Dream
PROJ_VERSION    := 0.0.1

BUILD           := build
TARGET          := $(PROJ_NAME)

# Absolute path to this Makefile's directory — survives recursive make from build/
PROJ_ROOT       := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

SOURCES         := src
INCLUDES        := include butano/common/include
DATA            :=
GRAPHICS        := butano/common/graphics
AUDIO           :=
FONTS           := $(PROJ_ROOT)/butano/template/fonts

BUTANO_HOME     := $(PROJ_ROOT)/butano/butano
LIBBUTANO       := $(BUTANO_HOME)
LIBBUTANOABS    := $(BUTANO_HOME)
include $(BUTANO_HOME)/butano.mak
