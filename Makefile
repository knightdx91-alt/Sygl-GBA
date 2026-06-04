PROJ_NAME       := sygl
PROJ_TITLE      := Sygl
PROJ_MAKER      := Dream
PROJ_VERSION    := 0.0.1

PROJ_SRCDIRS    := src $(BUTANO_HOME)/common
PROJ_INCDIRS    := include $(BUTANO_HOME)/common
PROJ_LIBRARIES  :=

BUTANO_HOME     ?= butano
include $(BUTANO_HOME)/butano.mak
