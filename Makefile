TARGET      :=  sygl_test
BUILD       :=  build
LIBBUTANO   :=  /project/butano/butano
PYTHON      :=  python
SOURCES     :=  src
INCLUDES    :=  include
DATA        :=
GRAPHICS    :=  graphics
AUDIO       :=  audio
DMGAUDIO    :=  dmg_audio
ROMTITLE    :=  SYGL TEST
ROMCODE     :=  SYGL

# Explicitly bypass 'realpath' to prevent Docker confusion
export LIBBUTANOABS := $(LIBBUTANO)

include $(LIBBUTANOABS)/butano.mak
