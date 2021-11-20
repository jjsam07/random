TARGET := iphone:clang:latest:7.0

include $(THEOS)/makefiles/common.mk

ARCHS = armv7 armv7s

TOOL_NAME = procmem

procmem_FILES = procmem.m
procmem_CFLAGS = -fobjc-arc
procmem_CODESIGN_FLAGS = -Sentitlements.plist
procmem_INSTALL_PATH = /usr/local/bin

procmem_LDFLAGS = -lncurses

include $(THEOS_MAKE_PATH)/tool.mk
include $(THEOS_MAKE_PATH)/tweak.mk
