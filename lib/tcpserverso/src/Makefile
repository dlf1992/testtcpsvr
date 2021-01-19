
include $(TOPDIR)/mk/config.mk


$(exlib): $(OBJS)
	$(AR) $(ARFLAGS) $(OBJ_DIR)/$@ $(OBJS)
	$(RANLIB) $(OBJ_DIR)/$@

$(exlibso):  $(OBJS)
	$(CXX) -shared -lpthread -o $(OBJ_DIR)/$@ $^

include $(TOPDIR)/mk/rules.mk

