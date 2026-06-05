PROJ_NAME       := sygl
PROJ_TITLE      := Sygl
PROJ_MAKER      := Dream
PROJ_VERSION    := 0.0.1

BUILD           := build
TARGET          := $(PROJ_NAME)

PROJ_SRCDIRS    := src
PROJ_INCDIRS    := include
PROJ_LIBRARIES  :=

BUTANO_HOME     ?= butano/butano
LIBBUTANO       := $(BUTANO_HOME)
LIBBUTANOABS    := $(abspath $(LIBBUTANO))
include $(BUTANO_HOME)/butano.mak
