THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CONFS = \
		$(THIS_DIR)/base.conf \
		$(THIS_DIR)/cpp.conf \
		$(THIS_DIR)/logging.conf \
		$(THIS_DIR)/usb.conf \
		$(THIS_DIR)/shell.conf 
		#$(THIS_DIR)/networking.conf
		#$(THIS_DIR)/mcumgr.conf \
		#$(THIS_DIR)/mcuboot.conf

$(THIS_DIR)/prj.conf: $(CONFS) $(THIS_DIR)/Makefile
	cat $(CONFS) > $@
	
