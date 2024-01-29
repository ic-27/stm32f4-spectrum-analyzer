PROJECT = audio_fft
BUILD_DIR = bin

SHARED_DIR = ../src/app/
CXXFILES = main.cpp
# CFILES += api.c
# AFILES += api-asm.S

DEVICE=stm32f407vgt6
OOCD_FILE = board/stm32f4discovery.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../lib/libopencm3

CXXSTD ?= -std=c++11
OPT = -O0
OOCD_INTERFACE = stlink
OOCD_TARGET = stm32f4x
include $(OPENCM3_DIR)/mk/genlink-config.mk
include ./rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
