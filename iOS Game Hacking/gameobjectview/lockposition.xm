#include <pthread.h>
#include "GameObjects.h"

Player **player = (Player **)0xB0DF84;
PlayerPosition0 *pos0 = NULL;
PlayerPosition1 *pos1 = NULL;

bool lockposition = false;
bool exiting = false;
bool start = false;
float _x, _y, z0, z1;

@interface LockPositionClass: NSObject <UIAlertViewDelegate, UITextViewDelegate> {
	UIButton *LockPositionButton;
	UIView *LockPositionView;
	UILongPressGestureRecognizer *LockPositionPress;
	NSString *buttonTitle;
	BOOL LockPositionViewVisible;
	BOOL DoOnce;
}
@end

@implementation LockPositionClass

-(void)SetLockPosition {
	if (lockposition) {
		lockposition = false;
		[LockPositionButton setTitle:@"LockPosition" forState: UIControlStateNormal];
	} else {
		lockposition = true;
		[LockPositionButton setTitle:@"UnLockPosition" forState: UIControlStateNormal];
	}
}

-(void)UpdateLockPosition {
	buttonTitle = [NSString stringWithFormat:@"X: %f\nY: %f\nZ: %f", _x, _y, z0];
	[LockPositionButton setTitle:buttonTitle forState: UIControlStateNormal];
}

-(void)Show_Hide_LockPositionView {
	if (!start) start = true; 
	if (!LockPositionViewVisible) {
		[[[UIApplication sharedApplication] keyWindow] addSubview:LockPositionView];
		if (DoOnce) {
			[LockPositionButton addGestureRecognizer:LockPositionPress];
			DoOnce = NO;
		}
		LockPositionViewVisible = YES;
	} else {
		[LockPositionView removeFromSuperview];
		LockPositionViewVisible = NO;
	}
}

-(void)LockPositionInit {
	LockPositionView = /*[*/[[UIView alloc] initWithFrame:CGRectMake(75, 84, 174, 84)] /*autorelease]*/;
	LockPositionView.backgroundColor = [UIColor blackColor];
	LockPositionView.layer.cornerRadius = 10;

	LockPositionButton = [UIButton buttonWithType: UIButtonTypeSystem];
	[LockPositionButton setTitle:@"LockPosition" forState: UIControlStateNormal];
	LockPositionButton.frame = CGRectMake(0, 0, 174, 84);
	LockPositionButton.backgroundColor = [UIColor blackColor];
	LockPositionButton.tintColor = [UIColor whiteColor];
	LockPositionButton.layer.cornerRadius = 10;
	LockPositionButton.titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
	LockPositionButton.titleLabel.textAlignment = NSTextAlignmentCenter;
	[LockPositionButton addTarget:self action:@selector(SetLockPosition) forControlEvents:UIControlEventTouchUpInside];
	[LockPositionView addSubview:LockPositionButton];

	LockPositionPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(UpdateLockPosition)];
	LockPositionPress.minimumPressDuration = .5;
	LockPositionPress.numberOfTouchesRequired = 1;
	
	LockPositionViewVisible = NO;
	DoOnce = YES;
}

@end

LockPositionClass *LockPositionClassInstance;

%hook UIWindow

-(void)becomeKeyWindow {
	%orig;
	
	UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:LockPositionClassInstance action:@selector(Show_Hide_LockPositionView)];
	tap.numberOfTouchesRequired = 4;
	tap.numberOfTapsRequired = 2;
	[self addGestureRecognizer:tap];
}

%end

void *lock_position_thread(void *) {	
	while(1) {
		if (start) {
			pos0 = (*player)->position0;
			pos1 = (*player)->position1;
			break;
		}
	}
	
	while(1) {
		if (exiting) break;
		if (lockposition) {
			//Get current position
			_x = pos0->XYZ_0[0];
			_y = pos0->XYZ_0[1];
			z0 = pos0->XYZ_0[2];
			z1 = pos1->z;
			while(lockposition) {
				pos0->XYZ_0[0] = _x;
				pos0->XYZ_1[0] = 0.0f;
				pos0->XYZ_2[0] = 0.0f;
				pos1->x = 0.0f;
				(*player)->playerX = 0.0f;
				pos0->XYZ_0[1] = _y;
				pos0->XYZ_1[1] = 0.0f;
				pos0->XYZ_2[1] = 0.0f;
				pos1->y = 0.0f;
				(*player)->playerY = 0.0f;
				pos0->XYZ_0[2] = z0;
				pos0->XYZ_1[2] = 0.0f;
				pos0->XYZ_2[2] = 0.0f;
				pos1->z = 0.0f;
				(*player)->playerZ = 0.0f;
			}
		}
	}
	
	return NULL;
}

%ctor {
	LockPositionClassInstance = [[%c(LockPositionClass) alloc] init];
	[LockPositionClassInstance LockPositionInit];
	
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, lock_position_thread, NULL);
	pthread_detach(thread_id);
}

%dtor {
	exiting = true;
}