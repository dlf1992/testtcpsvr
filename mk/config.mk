# cross_compiler
ifeq ($(origin CROSS_COMPILE), undefined)
 ifeq ($(PRJNAME), TM8765)
  CROSS_COMPILE = arm-hisiv500-linux-
 endif
endif	# end CORSS_COMPILER
#=================
#V1 := $(shell  echo "\033[40;31;25;1m CPUTYPE =\033[0m")
#V2 := $(shell  echo "\033[40;33;25;1m CPUTYPE =\033[0m")
#$(if $(CROSS_COMPILE),$(warning $(V1) $(CPUTYPE)), $(warning $(V2) $(CPUTYPE)))
#=================

AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
CXX =  $(CROSS_COMPILE)g++
LD  = $(CROSS_COMPILE)ld

STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB = $(CROSS_COMPILE)ranlib

ARFLAGS = crv

#=============================================

C_SOURCES := $(wildcard *.c)
CPP_SRCS1 := $(wildcard *.cpp)
CPP_SRCS2 := $(wildcard *.cc)

DEPS := $(addprefix $(OBJ_DIR)/, $(C_SOURCES:%.c=%.d))
DEPS += $(addprefix $(OBJ_DIR)/, $(CPP_SRCS1:%.cpp=%.d))
DEPS += $(addprefix $(OBJ_DIR)/, $(CPP_SRCS2:%.cc=%.d))

OBJS += $(patsubst %.d, %.o, $(DEPS))

#LIBS := $(notdir $(wildcard $(TOP_DIR)/lib/*.a))
#LIBS += $(notdir $(wildcard $(TOP_DIR)/lib/*.so))


