PRJNAME ?= 

# 根据需要，修改库文件名
lib_name = tcpsvr

CURRENT_DIR=$(shell pwd)
ROOT_DIR=$(shell dirname $(CURRENT_DIR))
#=================================================================
SRC_DIR=$(CURRENT_DIR)/src
INC_DIR = $(CURRENT_DIR)/include

libsdir = ../

export exlib = lib$(lib_name).a
export exlibso = lib$(lib_name).so

export OBJ_DIR=$(CURRENT_DIR)/obj

export TOPDIR ?= $(CURRENT_DIR)
$(shell [ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR))

SUB_SRC_DIR := $(shell ls $(SRC_DIR) -l | grep ^d | awk '{print $$9}')

include $(TOPDIR)/mk/file.mk
include $(TOPDIR)/mk/config.mk

CFLAGS += $(foreach dir,$(INC_DIR),-I$(dir))

LDFLAGS += -L$(CURRENT_DIR)/lib/$(CPUTYPE)  -L$(OBJ_DIR)

export  CFLAGS  LDFLAGS

#======================================================
#复制文件到制定目录下
#cpy_lib = chmod 777 $(OBJ_DIR)/$(word $(2), $(1));  cp -af $(OBJ_DIR)/$(word $(2), $(1)) $(3)
cpy_lib = cp -f $(OBJ_DIR)/$(word $(2), $(1)) $(3)

#====================================================
TARGET = $(exlib) $(exlibso)


OBJO=$(wildcard $(OBJ_DIR)/*.o)

.PHONY: $(TARGET) tags

all install:  $(TARGET)
	@#$(call cpy_lib, $(TARGET), 1, $(libsdir))
	$(call cpy_lib, $(TARGET), 2, $(libsdir))
	
	@echo "make time: $(CREAT_TIME)"

$(TARGET):
	@for dir in $(SUB_SRC_DIR) ; do \
		$(MAKE) -C $(SRC_DIR)/$$dir || exit "$$?"; done
	@$(MAKE) -C ./src $@

	@echo -e "\033[40;32;25;1m === Prg:$@,Project name: $(PRJNAME),Cpu type:$(CPUTYPE), end of make === \033[0m"

#	ctags -R --fields=+l ./
tags: 
	@ctags --fields=+l -w -o tags `find ./ \( -name '*.cc' -o \
	-name '*.[ch]' -o -name '*.cpp' \) -print`

cscope:
	@find ./ \( -name '*.cc' -o  -name '*.[ch]' \
		-o -name '*.cpp' \) -print > cscope.files
	@cscope -b -q -k

clean:
	-@#rm -f $(TARGET)
	-@rm -f $(OBJ_DIR)/*
	-@rm -f $(libsdir)/$(exlib)  $(libsdir)/$(exlibso)

include $(TOPDIR)/mk/rules.mk

