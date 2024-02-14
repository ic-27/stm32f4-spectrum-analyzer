ifndef SILENCE
	SILENCE = @
endif

MAKEFILE_DIR=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))
PROJECT_DIR=$(realpath $(MAKEFILE_DIR)..)
TEST_DIR=$(PROJECT_DIR)/test

CPPUTEST_HOME=$(TEST_DIR)/cpputest

# project source files path
SRC_DIRS=\
	$(PROJECT_DIR)/src/driver
	#$(PROJECT_DIR)/src/app\
	# $(PROJECT_DIR)/src/bsp\
	# $(PROJECT_DIR)/src/hal

SRC_FILES=\
	$(PROJECT_DIR)/lib/fatfs/source/glue.cpp

# project header files path
INCLUDE_DIRS=\
	$(PROJECT_DIR)/lib/fatfs/source\
	$(PROJECT_DIR)/src/driver
	#$(PROJECT_DIR)/src/app\
	# $(PROJECT_DIR)/src/bsp\
	# $(PROJECT_DIR)/src/hal

# add this to avoid problems when including cpputest headers on test files
INCLUDE_DIRS+=$(CPPUTEST_HOME)/include

## specify where the test code is located
TEST_SRC_DIRS=\
	$(TEST_DIR)/src/app\
	$(TEST_DIR)/src/bsp\
	$(TEST_DIR)/src/driver\
	$(TEST_DIR)/src/hal\
	$(TEST_DIR)/src/lib

CPPUTEST_CPPFLAGS += -DCPPUTEST_TEST
CPPUTEST_USE_EXTENSIONS = Y
CPPUTEST_WARNINGFLAGS =  -Wall -Wextra -Wshadow -Wswitch-default -Wswitch-enum -Wconversion -Wno-long-long
## what to call the test binary
TEST_TARGET=test_runner

# Colorize output
CPPUTEST_EXE_FLAGS += -c

include $(CPPUTEST_HOME)/build/MakefileWorker.mk
