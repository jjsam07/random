TARGET := iphone:clang:latest:7.0

include $(THEOS)/makefiles/common.mk

ARCHS = armv7

TWEAK_NAME = memoryedit

memoryedit_FILES = Tweak.xm
memoryedit_CFLAGS = -fobjc-arc

include $(THEOS_MAKE_PATH)/tweak.mk
