#include <pthread.h>
#include <unistd.h>
#include "memoryedit.h"
#include "detour.h"

typedef struct {
	char padding0[0x40];	//0x0
	float x;				//0x40
	float y;				//0x44
	float z;				//0x48
} PlayerPosition;

typedef struct {
	char padding0[0x58C];
	int health;
} CharacterObject;

typedef struct {
	char padding0[0x14]; 		//0x0
	PlayerPosition *position0;	//0x14
	char padding1[0x2E0]; 		//0x18
	PlayerPosition *position1; 	//0x2F8
	char padding2[0x290]; 		//0x2FC
	int maxHealth; 				//0x58C
	int health; 				//0x590
	char padding3[0xD4]; 		//0x594
	CharacterObject *CharObj;	//0x668
	char padding4[0x178];		//0x66C
	float x;					//0x7E4
	float y;					//0x7E8
	float z;					//0x7EC
} PlayerObject;

UITextView *textOutput;
UITextView *textOutput1;
UITextView *textOutput2;

int call_count = 0;

uint32_t ASLRslide = 0;
NSString *binary;
uintptr_t origPlayerInit = 0x501D29;
uintptr_t healthWriteOrig = 0x4A89F5;

PlayerObject *PlayerObj;
vm_address_t address;
BOOL textOutput2Visible;
int i = 0;
unsigned char buf[192];
size_t size = 192;

void playerInitHook();
void playerHealthWriteHook();
void healthWriteHook();

@interface MemoryEdit: NSObject <UIAlertViewDelegate, UITextViewDelegate> {
	UIAlertView *memoryEditAlert;
	UIButton *infoButton;
	UIButton *memoryEditButton;
	UIButton *textOutputButton;
	UIWindow *keyWindow;
	UIView *memoryEditView;
	CGFloat maxHeight;
	BOOL textOutputVisible;
}
@end

@implementation MemoryEdit

-(void)readMemory:(UIAlertView *)alertView {
	const char *address = [[alertView textFieldAtIndex:0].text UTF8String];
	const char *size = [[alertView textFieldAtIndex:1].text UTF8String];

	uint32_t addr = 0;
	size_t sz = 0;
	sscanf(size, "%ld", &sz);
	sscanf(address, "%x", &addr);

	char *protstr = infoProtectionLevel((vm_address_t)addr+ASLRslide);
	char *readdata = readMemory((vm_address_t)addr+ASLRslide, sz);

	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Read" message:[NSString stringWithFormat:@"%s\n0x%X: %s\nASLR slide: 0x%X", protstr, addr, readdata, ASLRslide] delegate:self cancelButtonTitle:@"Done" otherButtonTitles:nil];
	[alert show];
	[alert release];
	free(protstr);
	free(readdata);
}

-(void)readMemoryLive:(UIAlertView *)alertView {
	const char *addr = [[alertView textFieldAtIndex:0].text UTF8String];

	if (textOutput2Visible) {
		[textOutput2 removeFromSuperview];
		textOutput2Visible = NO;
	} else {
		sscanf(addr, "%x", &address);
		[keyWindow addSubview:textOutput2];
		textOutput2Visible = YES;
	}
}

-(void)writeMemory:(UIAlertView *)alertView {
	const char *address = [[alertView textFieldAtIndex:0].text UTF8String];
	const char *data = [[alertView textFieldAtIndex:1].text UTF8String];
	float floatData;
	int intData;
	unsigned int uintData;
	bool res;
	
	uint32_t addr = 0;
	
	int len = strlen(data);
	sscanf(address, "%x", &addr);
	
	if (data[0] == '0' && (data[1] == 'x' || data[1] == 'X')) {
		res = writeMemory(addr+ASLRslide, data);
	} else if (data[len-1] == 'f' || data[len-1] == 'F') {
		floatData = strtof(data, NULL);
		res = writeData(addr+ASLRslide, (vm_address_t)&floatData, sizeof(floatData));
	} else if (data[len-1] == 'u' || data[len-1] == 'U') {
		uintData = strtoul(data, NULL, 10);
		res = writeData(addr+ASLRslide, (vm_address_t)&uintData, sizeof(uintData));
	} else {
		intData = strtol(data, NULL, 10);
		res = writeData(addr+ASLRslide, (vm_address_t)&intData, sizeof(intData));
	}
	

	char *protstr = infoProtectionLevel((vm_address_t)addr+ASLRslide);
	char *origdata = readMemory((vm_address_t)addr+ASLRslide, strlen(data) / 2);

	NSString *msg;

	if(res) {
		msg = [NSString stringWithFormat:@"%s\nOriginal: %s\nAddress: 0x%@\nData: %@\nASLR slide: 0x%X", protstr, origdata, [[alertView textFieldAtIndex:0].text uppercaseString], [[alertView textFieldAtIndex:1].text uppercaseString], ASLRslide];
	} else {
		msg = [NSString stringWithFormat:@"Write failed\n%s\nOriginal: %s\nAddress: 0x%@\nData: %@\nASLR slide: 0x%X", protstr, origdata, [[alertView textFieldAtIndex:0].text uppercaseString], [[alertView textFieldAtIndex:1].text uppercaseString], ASLRslide];
	}

	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Write" message:msg delegate:self cancelButtonTitle:@"Done" otherButtonTitles:nil];
	[alert show];
	[alert release];
	free(protstr);
	free(origdata);
}

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (buttonIndex == 1) {
		[self writeMemory:alertView];
	} else if (buttonIndex == 2) {
		[self readMemory:alertView];
	} else if (buttonIndex == 3) {
		[self readMemoryLive:alertView];
	}
}

-(void)memoryEditAlertShow {
	[memoryEditAlert show];
}

-(void)showMemoryEdit {
	[[[UIApplication sharedApplication] keyWindow] addSubview:memoryEditView];
	keyWindow = [[UIApplication sharedApplication] keyWindow];
}

-(void)hideMemoryEdit {
	[memoryEditView removeFromSuperview];
}

-(void)showHideTextOutput {
	if (textOutputVisible) {
		[textOutput removeFromSuperview];
		[textOutput1 removeFromSuperview];
		textOutputVisible = NO;
	} else {
		[[[UIApplication sharedApplication] keyWindow] addSubview:textOutput];
		[[[UIApplication sharedApplication] keyWindow] addSubview:textOutput1];
		textOutputVisible = YES;
	}
}

-(void)printText:(NSString *)string withTextView:(UITextView *)textView {
	[textView insertText:string];
	[textView scrollRangeToVisible:NSMakeRange(textOutput.text.length - 1, 1)];
}

-(void)printTextOverwrite:(NSString *)string withTextView:(UITextView *)textView {
	textView.text = nil;
	[textView insertText:string];
}

-(void)moreInfo {
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:[NSString stringWithFormat:@"Executable: %@\nASLR slide: 0x%X", binary, ASLRslide] delegate:self cancelButtonTitle:@"Done" otherButtonTitles:nil];
	[alert show];
	[alert release];
}

-(void)memoryEditInit:(CGFloat)textOutputMaxHeight {
	memoryEditView = [[[UIView alloc] initWithFrame:CGRectMake(60, 50, 144, 42)] autorelease];
	memoryEditView.backgroundColor = [UIColor blackColor];
	memoryEditView.layer.cornerRadius = 10;

	memoryEditButton = [UIButton buttonWithType: UIButtonTypeSystem];
	[memoryEditButton setTitle:@"Memory" forState: UIControlStateNormal];
	memoryEditButton.frame = CGRectMake(0, 0, 78, 42);
	memoryEditButton.backgroundColor = [UIColor blackColor];
	memoryEditButton.tintColor = [UIColor whiteColor];
	memoryEditButton.layer.cornerRadius = 10;
	[memoryEditButton addTarget:self action:@selector(memoryEditAlertShow) forControlEvents:UIControlEventTouchUpInside];
	[memoryEditView addSubview:memoryEditButton];

	infoButton = [UIButton buttonWithType: UIButtonTypeInfoLight];
	infoButton.frame = CGRectMake(78, 10, 22, 22);
	[infoButton addTarget:self action:@selector(moreInfo) forControlEvents:UIControlEventTouchUpInside];
	[memoryEditView addSubview:infoButton];

	textOutputButton = [UIButton buttonWithType: UIButtonTypeSystem];
	[textOutputButton setTitle:@"T" forState: UIControlStateNormal];
	textOutputButton.frame = CGRectMake(111, 10, 22, 22);
	[textOutputButton addTarget:self action:@selector(showHideTextOutput) forControlEvents:UIControlEventTouchUpInside];
	[memoryEditView addSubview:textOutputButton];

	memoryEditAlert = [[UIAlertView alloc] initWithTitle:@"MemoryEdit" message:nil delegate:self
	cancelButtonTitle:@"Cancel" otherButtonTitles:@"Write", @"Read", @"ReadLive", nil];
	memoryEditAlert.alertViewStyle = UIAlertViewStyleLoginAndPasswordInput;

	[[memoryEditAlert textFieldAtIndex:1] setSecureTextEntry:NO];
	[[memoryEditAlert textFieldAtIndex:0] setPlaceholder:@"Address"];
	[[memoryEditAlert textFieldAtIndex:1] setPlaceholder:@"Data or size"];

	textOutput = [[UITextView alloc] initWithFrame:CGRectMake(60, 200, 190, textOutputMaxHeight)];
	textOutput1 = [[UITextView alloc] initWithFrame:CGRectMake(60, 305, 190, textOutputMaxHeight)];
	textOutput2 = [[UITextView alloc] initWithFrame:CGRectMake(0, 410, 320, 400)];
	textOutput2.font = [UIFont fontWithName:@"Courier" size: 14];

	textOutput.editable = NO;
	textOutput1.editable = NO;
	textOutput2.editable = NO;
	textOutputVisible = NO;
	textOutput2Visible = NO;
}

@end

MemoryEdit *memoryEditClass;

%hook UIWindow

-(void)becomeKeyWindow {
	%orig;

	UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:memoryEditClass action:@selector(showMemoryEdit)];
	longPress.minimumPressDuration = .5;
	longPress.numberOfTouchesRequired = 4;
	[self addGestureRecognizer:longPress];

	UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:memoryEditClass action:@selector(hideMemoryEdit)];
	tap.numberOfTouchesRequired = 2;
	tap.numberOfTapsRequired = 2;
	[self addGestureRecognizer:tap];
}

%end

%hook EAGLView

- (void)touchesEnded:(id)fp8 withEvent:(id)fp12 {
	if (PlayerObj != NULL){
		if (PlayerObj->CharObj != NULL) {
			PlayerObj->CharObj->health = 0;
		}
		[memoryEditClass printTextOverwrite:[NSString stringWithFormat:@"%p X: %f\n%p Y: %f\n%p Z: %f\n%p X: %f\n%p Y: %f\n%p Z: %f\n", 
		&(PlayerObj->position0->x), PlayerObj->position0->x, 
		&(PlayerObj->position0->y), PlayerObj->position0->y, 
		&(PlayerObj->position0->z), PlayerObj->position0->z, 
		&(PlayerObj->position1->x), PlayerObj->position1->x, 
		&(PlayerObj->position1->y), PlayerObj->position1->y, 
		&(PlayerObj->position1->z), PlayerObj->position1->z] withTextView:textOutput1];
	}
	if (textOutput2Visible) {
		vm_read_overwrite(mach_task_self(), address, size, (vm_address_t)&buf, (vm_size_t *)&size);
		textOutput2.text = nil;
		for(int i = 0; i < size; i += 8) {
			[memoryEditClass printText:[NSString stringWithFormat:@"0x%X: %02hhX %02hhX %02hhX %02hhX   %02hhX %02hhX %02hhX %02hhX\n", address+i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]] withTextView:textOutput2];
		}
	}
	%orig;
}

- (void)touchesMoved:(id)fp8 withEvent:(id)fp12 {
	if (PlayerObj != NULL){
		if (PlayerObj->CharObj != NULL) {
			PlayerObj->CharObj->health = 0;
		}
		[memoryEditClass printTextOverwrite:[NSString stringWithFormat:@"%p X: %f\n%p Y: %f\n%p Z: %f\n%p X: %f\n%p Y: %f\n%p Z: %f\n", 
		&(PlayerObj->position0->x), PlayerObj->position0->x, 
		&(PlayerObj->position0->y), PlayerObj->position0->y, 
		&(PlayerObj->position0->z), PlayerObj->position0->z, 
		&(PlayerObj->position1->x), PlayerObj->position1->x, 
		&(PlayerObj->position1->y), PlayerObj->position1->y, 
		&(PlayerObj->position1->z), PlayerObj->position1->z] withTextView:textOutput1];
	}
	if (textOutput2Visible) {
		vm_read_overwrite(mach_task_self(), address, size, (vm_address_t)&buf, (vm_size_t *)&size);
		textOutput2.text = nil;
		for(int i = 0; i < size; i += 8) {
			[memoryEditClass printText:[NSString stringWithFormat:@"0x%X: %02hhX %02hhX %02hhX %02hhX   %02hhX %02hhX %02hhX %02hhX\n", address+i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]] withTextView:textOutput2];
		}
	}
	%orig;
}

- (void)touchesBegan:(id)fp8 withEvent:(id)fp12 {
	if (PlayerObj != NULL){
		if (PlayerObj->CharObj != NULL) {
			PlayerObj->CharObj->health = 0;
		}
		[memoryEditClass printTextOverwrite:[NSString stringWithFormat:@"%p X: %f\n%p Y: %f\n%p Z: %f\n%p X: %f\n%p Y: %f\n%p Z: %f\n", 
		&(PlayerObj->position0->x), PlayerObj->position0->x, 
		&(PlayerObj->position0->y), PlayerObj->position0->y, 
		&(PlayerObj->position0->z), PlayerObj->position0->z, 
		&(PlayerObj->position1->x), PlayerObj->position1->x, 
		&(PlayerObj->position1->y), PlayerObj->position1->y, 
		&(PlayerObj->position1->z), PlayerObj->position1->z] withTextView:textOutput1];
	}
	if (textOutput2Visible) {
		vm_read_overwrite(mach_task_self(), address, size, (vm_address_t)&buf, (vm_size_t *)&size);
		textOutput2.text = nil;
		for(int i = 0; i < size; i += 8) {
			[memoryEditClass printText:[NSString stringWithFormat:@"0x%X: %02hhX %02hhX %02hhX %02hhX   %02hhX %02hhX %02hhX %02hhX\n", address+i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]] withTextView:textOutput2];
		}
	}
	%orig;
}

%end

extern "C" {
int playerPrint(void *arg) {
	call_count++;
	if (call_count == 2) PlayerObj = (PlayerObject *)arg;
	[memoryEditClass printText:[NSString stringWithFormat:@"PlayerObj @ %p\n", arg] withTextView:textOutput];
	return 0;
}
}
__attribute__((naked, optnone)) void playerInitHook() {
	asm volatile("push {r0-r11, lr}");
	asm volatile("bl _playerPrint");
	asm volatile("mov r12, %0"::"r" (origPlayerInit));
	asm volatile("pop {r0-r11, lr}");
	asm volatile("bx r12");
}

//PlayerObject + 0x58C: Health
//PlayerObject + 0x590: Health
//PlayerObject + 0x7E4: X-coordinate
//PlayerObject + 0x7E8: Y-coordinate
//PlayerObject + 0x7EC: Z-coordinate
%ctor {
	CGFloat h = 100;
	binary = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleExecutable"];
	ASLRslide = getSlide([binary UTF8String]);
	memoryEditClass = [[%c(MemoryEdit) alloc] init];
	[memoryEditClass memoryEditInit:h];
	[memoryEditClass printText:[NSString stringWithFormat:@"PlayerObj pointer @ %p\n", &PlayerObj] withTextView:textOutput];
	detour((void *)0x474FAE, (void *)playerInitHook);
}