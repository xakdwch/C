zkbase_DIR=$(ROOT_DIR)/common/zkbase/
zkthread_DIR=$(ROOT_DIR)/common/zkthread/

distriqueue_DIR=$(ROOT_DIR)/demo/distriqueue/
subscriber_DIR=$(ROOT_DIR)/demo/subscribe/

tool_DIR=$(ROOT_DIR)/tool/

.PHONY=all
all:
	@echo "Start Compile common ..."
	$(MAKE) -C $(zkbase_DIR)
	$(MAKE) -C $(zkthread_DIR)

	@echo "Start Compile demon ..."
	$(MAKE) -C $(distriqueue_DIR)
	$(MAKE) -C $(subscriber_DIR)

	@echo "Start Compile tool ..."
	$(MAKE) -C $(tool_DIR)
	@echo "Make all done."

.PHONY=clean
clean:
	@echo "Start clean common ..."
	$(MAKE) clean -C $(zkbase_DIR)
	$(MAKE) clean -C $(zkthread_DIR)
	@echo "End clean common ..."

	@echo "Start clean demo ..."
	$(MAKE) clean -C $(distriqueue_DIR)
	$(MAKE) clean -C $(subscriber_DIR)
	@echo "End clean demo ..."

	@echo "Start clean tool ..."
	$(MAKE) clean -C $(tool_DIR)
	@echo "End clean tool ..."

	rm -f $(ROOT_DIR)/include/*.h
	rm -f $(ROOT_DIR)/lib/*.so
	@echo "Clean all done."
