KERNEL_DIR=/home/jjsam07/a7lte_cm_13/

CROSS_COMPILE=/home/jjsam07/arm-eabi-4.8/bin/arm-eabi-

EXTRAVERSION = -g9039320
CFLAGS_hello.o :=

obj-m += erasto.o
$(obj-m:.o=)-objs := hello.o
PWD := $(shell pwd)
default:
	$(MAKE) ARCH=arm CROSS_COMPILE=/home/jjsam07/arm-eabi-4.8/bin/arm-eabi- -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) clean
strip:
	$(CROSS_COMPILE)strip -S --strip-unneeded $(PWD)/$(obj-m:.o=).ko
	