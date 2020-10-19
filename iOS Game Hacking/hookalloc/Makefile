ARCHS = armv7 armv7s

THEOS_BUILD_DIR = build

include theos/makefiles/common.mk

TWEAK_NAME = hookmalloc hookcalloc hookrealloc
hookmalloc_FILES = malloc.xm
hookcalloc_FILES = calloc.xm
hookrealloc_FILES = realloc.xm

include $(THEOS_MAKE_PATH)/tweak.mk


