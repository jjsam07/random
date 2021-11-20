#include <pthread.h>
#include "GameObjects.h"

typedef struct {
	char padding[0x2C];
	float x;
	float y;
} PositionXY;

typedef struct {
	char padding[0x44];
	PositionXY *xy;
} PositionPointer;

PositionPointer **position_ptr = (PositionPointer **)0xB09420;
Player **player = (Player **)0xB0DF84;

float _x, _y, _z;

@interface TeleportClass: NSObject <UIAlertViewDelegate, UITextViewDelegate> {
	UIAlertController *TeleportAlertController;
	UIAlertAction *TeleportAction;
	UIAlertAction *TeleportActionCancel;
	UIButton *TeleportButton;
	UIView *TeleportView;
	UIWindow *topWindow;
	BOOL TeleportViewVisible;
}
@end

@implementation TeleportClass

-(void)TeleportShowAlert {
	topWindow.hidden = NO;
	topWindow.windowLevel = UIWindowLevelAlert + 1;
	[topWindow makeKeyAndVisible];
	[topWindow.rootViewController presentViewController:TeleportAlertController animated:YES completion:nil];
}

-(void)Show_Hide_TeleportView /* :(UILongPressGestureRecognizer *)gesture */ {
	//if (gesture.state == UIGestureRecognizerStateBegan) {
		if (!TeleportViewVisible) {
			[[[UIApplication sharedApplication] keyWindow] addSubview:TeleportView];
			TeleportViewVisible = YES;
		} else {
			[TeleportView removeFromSuperview];
			TeleportViewVisible = NO;
		}
	//}
}

-(void)TeleportInit {
	TeleportView = /*[*/[[UIView alloc] initWithFrame:CGRectMake(75, 84, 110, 42)] /*autorelease]*/;
	TeleportView.backgroundColor = [UIColor blackColor];
	TeleportView.layer.cornerRadius = 10;

	TeleportButton = [UIButton buttonWithType: UIButtonTypeSystem];
	[TeleportButton setTitle:@"Teleport" forState: UIControlStateNormal];
	TeleportButton.frame = CGRectMake(0, 0, 110, 42);
	TeleportButton.backgroundColor = [UIColor blackColor];
	TeleportButton.tintColor = [UIColor whiteColor];
	TeleportButton.layer.cornerRadius = 10;
	TeleportButton.titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
	TeleportButton.titleLabel.textAlignment = NSTextAlignmentCenter;
	[TeleportButton addTarget:self action:@selector(TeleportShowAlert) forControlEvents:UIControlEventTouchUpInside];
	[TeleportView addSubview:TeleportButton];
	
	TeleportAlertController = [UIAlertController alertControllerWithTitle:@"Teleport" message:nil preferredStyle:UIAlertControllerStyleAlert];
	
	[TeleportAlertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
    textField.placeholder = @"X";
	}];
	
	[TeleportAlertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
    textField.placeholder = @"Y";
	}];
	
	[TeleportAlertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
    textField.placeholder = @"Z";
	}];
	
	TeleportAction = [UIAlertAction actionWithTitle:@"GO" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
		_x = strtof([[[TeleportAlertController textFields][0] text] UTF8String], NULL) / 100.0f;
		_y = strtof([[[TeleportAlertController textFields][1] text] UTF8String], NULL) / 100.0f;
		_z = strtof([[[TeleportAlertController textFields][2] text] UTF8String], NULL);
		
		(*player)->position0->XYZ_0[2] = _z;
		(*position_ptr)->xy->x = _x;
		(*position_ptr)->xy->y = _y;
		
		topWindow.hidden = YES;
	}];
	
	TeleportActionCancel = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
		topWindow.hidden = YES;
	}];
	
	[TeleportAlertController addAction:TeleportAction];
	[TeleportAlertController addAction:TeleportActionCancel];
	
	topWindow = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	topWindow.rootViewController = [UIViewController new];
	topWindow.windowLevel = UIWindowLevelAlert + 1;
	
	TeleportViewVisible = NO;
}

@end

TeleportClass *TeleportInstance;

%hook UIWindow

-(void)becomeKeyWindow {
	%orig;
	
	UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:TeleportInstance action:@selector(Show_Hide_TeleportView)];
	tap.numberOfTouchesRequired = 4;
	tap.numberOfTapsRequired = 2;
	[self addGestureRecognizer:tap];
}

%end

%ctor {
	TeleportInstance = [[%c(TeleportClass) alloc] init];
	[TeleportInstance TeleportInit];
}