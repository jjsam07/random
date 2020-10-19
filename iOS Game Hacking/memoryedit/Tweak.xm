#include <pthread.h>
#include <unistd.h>
#include "memoryedit.h"
#include "detour.h"

UITextView *textOutput;
UITextView *textOutput1;

int call_count = 0;

uint32_t ASLRslide = 0;
NSString *binary;
uintptr_t origPlayerInit = 0x501D29;
uintptr_t healthWriteOrig = 0x4A89F5;

uintptr_t playerObject = 0;
uintptr_t playerObject1 = 0;
uintptr_t playerObject2 = 0;

void playerInitHook();
void playerHealthWriteHook();
void healthWriteHook();

@interface MemoryEdit: NSObject <UIAlertViewDelegate, UITextViewDelegate> {
	UIAlertView *memoryEditAlert;
	UIButton *infoButton;
	UIButton *memoryEditButton;
	UIButton *textOutputButton;
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

-(void)writeMemory:(UIAlertView *)alertView {
	const char *address = [[alertView textFieldAtIndex:0].text UTF8String];
	const char *data = [[alertView textFieldAtIndex:1].text UTF8String];
	uint32_t addr = 0;

	sscanf(address, "%x", &addr);

	char *protstr = infoProtectionLevel((vm_address_t)addr+ASLRslide);
	char *origdata = readMemory((vm_address_t)addr+ASLRslide, strlen(data) / 2);

	NSString *msg;

	if(writeMemory(addr+ASLRslide, data)) {
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
	}
}

-(void)memoryEditAlertShow {
	[memoryEditAlert show];
}

-(void)showMemoryEdit {
	[[[UIApplication sharedApplication] keyWindow] addSubview:memoryEditView];
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
	memoryEditView = [[[UIView alloc] initWithFrame:CGRectMake(60, 50, 177, 42)] autorelease];
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
	cancelButtonTitle:@"Cancel" otherButtonTitles:@"Write", @"Read", nil];
	memoryEditAlert.alertViewStyle = UIAlertViewStyleLoginAndPasswordInput;

	[[memoryEditAlert textFieldAtIndex:1] setSecureTextEntry:NO];
	[[memoryEditAlert textFieldAtIndex:0] setPlaceholder:@"Address"];
	[[memoryEditAlert textFieldAtIndex:1] setPlaceholder:@"Data or size"];

	textOutput = [[UITextView alloc] initWithFrame:CGRectMake(60, 250, 190, textOutputMaxHeight)];
	textOutput1 = [[UITextView alloc] initWithFrame:CGRectMake(60, 355, 190, textOutputMaxHeight)];

	textOutput.editable = NO;
	textOutput1.editable = NO;
	textOutputVisible = NO;
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
	if (*(int *)(playerObject1+0x668) != 0) {
		*(int *)(*(int *)(playerObject1+0x668)+0x58C) = 0;
	}
	[memoryEditClass printTextOverwrite:[NSString stringWithFormat:@"X: %i\nY: %i\nZ: %i\nX: %f\nY: %f\nZ: %f\n", *(unsigned int *)(playerObject1+0x7e4), *(unsigned int *)(playerObject1+0x7e8), *(unsigned int *)(playerObject1+0x7ec), *(float *)(playerObject1+0x7e4), *(float *)(playerObject1+0x7e8), *(float *)(playerObject1+0x7ec)] withTextView:textOutput1];
	%orig;
}

- (void)touchesMoved:(id)fp8 withEvent:(id)fp12 {
	if (*(int *)(playerObject1+0x668) != 0) {
		*(int *)(*(int *)(playerObject1+0x668)+0x58C) = 0;
	}
	[memoryEditClass printTextOverwrite:[NSString stringWithFormat:@"X: %i\nY: %i\nZ: %i\nX: %f\nY: %f\nZ: %f\n", *(unsigned int *)(playerObject1+0x7e4), *(unsigned int *)(playerObject1+0x7e8), *(unsigned int *)(playerObject1+0x7ec), *(float *)(playerObject1+0x7e4), *(float *)(playerObject1+0x7e8), *(float *)(playerObject1+0x7ec)] withTextView:textOutput1];
	%orig;
}

- (void)touchesBegan:(id)fp8 withEvent:(id)fp12 {
	if (*(int *)(playerObject1+0x668) != 0) {
		*(int *)(*(int *)(playerObject1+0x668)+0x58C) = 0;
	}
	[memoryEditClass printTextOverwrite:[NSString stringWithFormat:@"X: %i\nY: %i\nZ: %i\nX: %f\nY: %f\nZ: %f\n", *(unsigned int *)(playerObject1+0x7e4), *(unsigned int *)(playerObject1+0x7e8), *(unsigned int *)(playerObject1+0x7ec), *(float *)(playerObject1+0x7e4), *(float *)(playerObject1+0x7e8), *(float *)(playerObject1+0x7ec)] withTextView:textOutput1];
	%orig;
}

%end

extern "C" {
int playerPrint(void *arg) {
	call_count++;
	if (call_count == 2) playerObject1 = (uintptr_t)arg;
	//[memoryEditClass printText:[NSString stringWithFormat:@"PlayerObj @ %p\n", arg] withTextView:textOutput];
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
	[memoryEditClass printText:[NSString stringWithFormat:@"PlayerObj pointer @ %p\n", &playerObject] withTextView:textOutput];
	detour((void *)0x474FAE, (void *)playerInitHook);
	//detour((void *)0x501D10, (void *)playerHealthWriteHook);
	//detour((void *)0x47D72E, (void *)healthWriteHook);
}