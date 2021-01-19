#==================================================
ifeq ($(PRJNAME), TM8765)
  CPUTYPE = hi3531d
endif

#==================================================
#
CFLAGS = -D$(PRJNAME) -D$(CPUTYPE) -D__LINUX__
CFLAGS += -g -rdynamic -Wall
CFLAGS += -fPIC
CFLAGS += -O2 -fno-strict-aliasing -finline-functions -std=c++11
CFLAGS += -D_REENTRANT

CREAT_TIME= $(shell date "+%F %T")


export CFLAGS LDFLAGS PRJNAME CPUTYPE CREAT_TIME 
