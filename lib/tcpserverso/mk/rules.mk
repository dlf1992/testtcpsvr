sinclude $(DEPS)

ifneq ($(SEL_CXX), 1)
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
else
$(OBJ_DIR)/%.o: %.c
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
endif

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cc
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@


$(OBJ_DIR)/%.d :%.c
	@set -e; rm -f $@; \
		$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,$(OBJ_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

$(OBJ_DIR)/%.d :%.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJ_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJ_DIR)/%.d :%.cc
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJ_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$



