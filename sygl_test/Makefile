#---------------------------------------------------------------------------------------------------------------------
TARGET      :=  sygl_test
BUILD       :=  build
LIBBUTANO   :=  butano/butano
PYTHON      :=  python
SOURCES     :=  src
INCLUDES    :=  include
DATA        :=
GRAPHICS    :=  graphics
AUDIO       :=  audio
DMGAUDIO    :=  dmg_audio
ROMTITLE    :=  SYGL TEST
ROMCODE     :=  SYGL
USERFLAGS   :=  
USERASFLAGS :=  
USERLDFLAGS :=  
USERLIBDIRS :=  
USERLIBS    :=  
USERBUILD   :=  
EXTTOOL     :=  

#---------------------------------------------------------------------------------------------------------------------
# Export absolute butano path:
#---------------------------------------------------------------------------------------------------------------------
ifndef LIBBUTANO
    $(error "Please set LIBBUTANO in your makefile")
endif

export LIBBUTANOABS := $(realpath $(LIBBUTANO))

#---------------------------------------------------------------------------------------------------------------------
# Include main makefile:
#---------------------------------------------------------------------------------------------------------------------
include $(LIBBUTANOABS)/butano.mak
