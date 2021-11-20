TARGET := iphone:clang:latest:7.0

include $(THEOS)/makefiles/common.mk

TOOL_NAME = weapon playerposition teleport
TWEAK_NAME = lockposition teleport_tweak

ARCHS = armv7

weapon_FILES = weapon.m
weapon_CFLAGS = -fobjc-arc
weapon_CODESIGN_FLAGS = -Sentitlements.plist
weapon_INSTALL_PATH = /usr/local/bin

weapon_LDFLAGS = -lncurses


playerposition_FILES = playerposition.m
playerposition_CFLAGS = -fobjc-arc
playerposition_CODESIGN_FLAGS = -Sentitlements.plist
playerposition_INSTALL_PATH = /usr/local/bin

playerposition_LDFLAGS = -lncurses


teleport_FILES = teleport.m
teleport_CFLAGS = -fobjc-arc
teleport_CODESIGN_FLAGS = -Sentitlements.plist
teleport_INSTALL_PATH = /usr/local/bin

teleport_LDFLAGS = -lncurses


lockposition_FILES = lockposition.xm
lockposition_CFLAGS = -fobjc-arc

teleport_tweak_FILES = teleport.xm
teleport_tweak_CFLAGS = -fobjc-arc

include $(THEOS_MAKE_PATH)/tool.mk
include $(THEOS_MAKE_PATH)/tweak.mk
