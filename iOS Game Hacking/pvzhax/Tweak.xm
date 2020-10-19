#include "aslrwrite.h"

uint32_t ASLRslide = 0;

%ctor {
ASLRslide = getSlide("PvZ");
writeData(ASLRslide+0x19EB88, "00BF"); //cooldown
writeData(ASLRslide+0xA2908, "00BF"); //sun - spend
writeData(ASLRslide+0xB5560, "1A60"); //sun - add
}