#Author : dlf
#Time: 2020/12/21

PRJNAME ?= TM8765
TARGET = testtcpsvr
CURDIR=$(shell pwd)
BIN_DIR=$(CURDIR)/bin

SRC_DIR = $(CURDIR)/src

INC_DIR = $(CURDIR)/include 

#INC_DIR += /mnt/nand/hisiv500/zlog-1.2.15/include

LIB_DIR=$(CURDIR)/lib
#LIB_DIR += /mnt/nand/hisiv500/zlog-1.2.15/lib

$(shell [ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR))

export OBJ_DIR=$(CURDIR)/obj

export TOPDIR ?= $(CURDIR)
$(shell [ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR))

SUB_LIB_DIR := $(shell ls $(LIB_DIR) -l | grep ^d | awk '{print $$9}')

SUB_SRC_DIR := $(SRC_DIR1)
SUB_SRC_DIR += $(shell for dir in $(SRC_DIR1) ; do cd $${dir}; ls -l|grep ^d|awk '{print $$9}'|awk '{print i$$0}' i=`pwd`'/'; done)

include $(TOPDIR)/mk/file.mk
include $(TOPDIR)/mk/config.mk

CFLAGS += $(foreach dir,$(INC_DIR),-I$(dir))
LDFLAGS += $(foreach dir,$(LIB_DIR),-L$(dir))


#优化程序体积
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

CFLAGS += $(foreach dir,$(INC_DIR),-I$(dir))
SUB_INC_DIR := $(shell for dir in $(INC_DIR) ; do cd $${dir}; ls -l|grep ^d|awk '{print $$9}'|awk '{print i$$0}' i=`pwd`'/'; done)
CFLAGS += $(foreach dir,$(SUB_INC_DIR),-I$(dir))


LDFLAGS += -lpthread 
#LDFLAGS += $(wildcard $(LIB_DIR)/*.a)
LDFLAGS += -L$(LIB_DIR)
LDFLAGS += -ltcpsvr

export  CFLAGS  LDFLAGS

#====================================================

OBJO=$(wildcard $(OBJ_DIR)/*.o)

.PHONY: $(TARGET) tags

export SEL_CXX=1

all install:  $(TARGET)
	@export SEL_CXX=0
	@echo "make time: $(CREAT_TIME)"

$(TARGET): libs make_objs
	#$(CXX) -std=c++11  $(OBJO) -o $@ $(LDFLAGS)
	$(CXX) $(OBJO) -o $@ $(LDFLAGS)
	$(STRIP) $@
	@cp -f $@ $(BIN_DIR)
	@echo -e "\033[40;32;25;1m === Prg:$@,Project name: $(PRJNAME),Cpu type:$(CPUTYPE), end of make === \033[0m"

libs:
	@for dir in $(SUB_LIB_DIR) ; do \
		cd $(LIB_DIR)/$$dir;\
		$(MAKE) || exit "$$?"; done
		
make_objs:
	@for dir in $(SUB_SRC_DIR) ; do \
		$(MAKE) -C $$dir || exit "$$?"; done
	@$(MAKE) -C $(SRC_DIR)

#	ctags -R --fields=+l ./
tags: 
	@ctags --fields=+l -w -o tags `find ./ \( -name '*.cc' -o \
	-name '*.[ch]' -o -name '*.cpp' \) -print`

cscope:
	@find ./ \( -name '*.cc' -o  -name '*.[ch]' \
		-o -name '*.cpp' \) -print > cscope.files
	@cscope -b -q -k



clean:
	-@rm -f $(TARGET)
	-@rm -rf $(BIN_DIR)/*
	-@rm -rf $(OBJ_DIR)/*
	@for dir in $(SUB_LIB_DIR) ; do \
		cd $(LIB_DIR)/$$dir;\
		$(MAKE) clean; done

print:	
	@echo  =====$(INC_DIR)======
print:
	@echo  =====$(CFLAGS)======

include $(TOPDIR)/mk/rules.mk

