PROJECT = audio_fft
BUILD_DIR = bin

SHARED_DIR += ../src/app/ ../src/driver
CXXFILES += main.cpp executor.cpp oled.cpp

DEVICE=stm32f407vgt6
OOCD_FILE = board/stm32f4discovery.cfg

CXXSTD ?= -std=c++11
OPT = -O0
OOCD_INTERFACE = stlink
OOCD_TARGET = stm32f4x

# add fatfs
SHARED_DIR += ../lib/fatfs/source/
INCLUDES += -I../lib/fatfs/source/
CXXFILES += glue.cpp
CFILES += ff.c diskio.c

# link w/ CMSIS 4 for DSP
CMSIS_HEADER_PATH = ../lib/CMSIS_4/CMSIS/Include #/arm_math.h
CMSIS_LIB_PATH = ../lib/CMSIS_4/CMSIS/Lib/GCC #libarm_cortexM4l_math.a

INCLUDES += -I$(CMSIS_HEADER_PATH) # where to find arm_math.h
TGT_LDFLAGS += -L$(CMSIS_LIB_PATH) # searchdir
LDLIBS += -larm_cortexM4lf_math # lib in searchdir

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../lib/libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ./rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
