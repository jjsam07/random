TARGET := iphone:clang:latest:7.0

include $(THEOS)/makefiles/common.mk

TOOL_NAME = weapon

ARCHS = armv7

weapon_FILES = weapon.m
weapon_CFLAGS = -fobjc-arc
weapon_CODESIGN_FLAGS = -Sentitlements.plist
weapon_INSTALL_PATH = /usr/local/bin

weapon_LDFLAGS = -lncurses

include $(THEOS_MAKE_PATH)/tool.mk
