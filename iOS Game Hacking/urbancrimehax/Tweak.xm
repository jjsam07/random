#include <aslrwrite.h>

@interface gliveViewController
	-(void)showAlertWithTitle:(NSString *)title andMessage:(NSString *)message;
@end

/*   Global Variables   */
bool carHealth = false;
bool weaponMag = false;
bool health = false;
bool ammo = false;
bool damage = false;
UISwitch *weaponMagSwitch;
UISwitch *carHealthSwitch;
UISwitch *healthSwitch;
UISwitch *ammoSwitch;
UISwitch *damageSwitch;
UISwitch *setToZero;
UIView *cheatMenu;
static bool isCalled = false;
void *PlayerObject0;
void *PlayerObject1; // Main PlayerObject 
					 // PlayerObject pointer at 0xB0DF84
void *PlayerObject2;

#include "hooks.h"

%hook gliveViewController

- (void)quitGLIVE:(id)fp8{
	isCalled = false;
	%orig;
}

- (void)webViewDidStartLoad:(id)fp8{

	if (!isCalled){
		isCalled = true;

		UIViewController *uiview = MSHookIvar<UIViewController*>(self, "m_subVC");

		UIButton *cheatsButton = [UIButton buttonWithType:UIButtonTypeSystem];
		[cheatsButton setTitle:@"Cheats" forState:UIControlStateNormal]; 
		cheatsButton.frame = CGRectMake(1, 1, 80, 42);
		cheatsButton.backgroundColor = [UIColor whiteColor];
		cheatsButton.tintColor = [UIColor blackColor];
		cheatsButton.layer.cornerRadius = 10;
		[cheatsButton addTarget:self action:@selector(showMenu) forControlEvents:UIControlEventTouchUpInside];
		[uiview.view addSubview:cheatsButton];
	}
	%orig;
}

%new
-(void)showMenu {

	UIActivityIndicatorView *indicator = MSHookIvar<UIActivityIndicatorView*>(self, "activityIndicator");
	[indicator removeFromSuperview];

	UIImageView *blank = MSHookIvar<UIImageView*>(self, "blankView");
	[blank removeFromSuperview];

	UIViewController *uiview = MSHookIvar<UIViewController*>(self, "m_subVC");

	cheatMenu = [[[UIView alloc] initWithFrame:CGRectMake(0, 0, 640, 1136)] autorelease];
	cheatMenu.backgroundColor = [UIColor blackColor];

	UIButton *exitButton = [UIButton buttonWithType:UIButtonTypeSystem];
	[exitButton setTitle:@"Exit" forState:UIControlStateNormal]; 
	exitButton.frame = CGRectMake(239, 1, 80, 42);
	exitButton.backgroundColor = [UIColor whiteColor];
	exitButton.tintColor = [UIColor blackColor];
	exitButton.layer.cornerRadius = 10;
	[exitButton addTarget:self action:@selector(exitMenu) forControlEvents:UIControlEventTouchUpInside];

	weaponMagSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(250, 50, 0, 0)];
	[weaponMagSwitch setOn:weaponMag animated:YES];
	[weaponMagSwitch addTarget:self action:@selector(weaponMagCheat) forControlEvents:UIControlEventValueChanged];

	UILabel *weaponMagLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 50, 229, 31)];
	weaponMagLabel.text = @"Weapon magazine";
	weaponMagLabel.backgroundColor = [UIColor clearColor];
	weaponMagLabel.textColor = [UIColor whiteColor];
	weaponMagLabel.textAlignment = UITextAlignmentLeft;

	carHealthSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(250, 100, 0, 0)];
	[carHealthSwitch setOn:carHealth animated:YES];
	[carHealthSwitch addTarget:self action:@selector(carHealthCheat) forControlEvents:UIControlEventValueChanged];
	
	UILabel *carHealthLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 100, 229, 31)];
	carHealthLabel.text = @"Car health";
	carHealthLabel.backgroundColor = [UIColor clearColor];
	carHealthLabel.textColor = [UIColor whiteColor];
	carHealthLabel.textAlignment = UITextAlignmentLeft;
	
	ammoSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(250, 150, 0, 0)];
	[ammoSwitch setOn:ammo animated:YES];
	[ammoSwitch addTarget:self action:@selector(ammoCheat) forControlEvents:UIControlEventValueChanged];
	
	UILabel *ammoLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 150, 229, 31)];
	ammoLabel.text = @"Unlimited ammo";
	ammoLabel.backgroundColor = [UIColor clearColor];
	ammoLabel.textColor = [UIColor whiteColor];
	ammoLabel.textAlignment = UITextAlignmentLeft;
	
	healthSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(250, 200, 0, 0)];
	[healthSwitch setOn:health animated:YES];
	[healthSwitch addTarget:self action:@selector(healthCheat) forControlEvents:UIControlEventValueChanged];
	
	UILabel *healthLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 200, 229, 31)];
	healthLabel.text = @"Invincible";
	healthLabel.backgroundColor = [UIColor clearColor];
	healthLabel.textColor = [UIColor whiteColor];
	healthLabel.textAlignment = UITextAlignmentLeft;
	
	damageSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(250, 250, 0, 0)];
	[damageSwitch setOn:damage animated:YES];
	[damageSwitch addTarget:self action:@selector(damageCheat) forControlEvents:UIControlEventValueChanged];
	
	UILabel *damageLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 250, 229, 31)];
	damageLabel.text = @"Infinite Damage";
	damageLabel.backgroundColor = [UIColor clearColor];
	damageLabel.textColor = [UIColor whiteColor];
	damageLabel.textAlignment = UITextAlignmentLeft;
	
	UILabel *playerObject0Label = [[UILabel alloc] initWithFrame:CGRectMake(20, 300, 300, 31)];
	playerObject0Label.text = [NSString stringWithFormat:@"PlayerObject0: %p", PlayerObject0];
	playerObject0Label.backgroundColor = [UIColor clearColor];
	playerObject0Label.textColor = [UIColor whiteColor];
	playerObject0Label.textAlignment = UITextAlignmentLeft;
	
	UILabel *playerObject1Label = [[UILabel alloc] initWithFrame:CGRectMake(20, 350, 300, 31)];
	playerObject1Label.text = [NSString stringWithFormat:@"PlayerObject1: %p (main)", PlayerObject1];
	playerObject1Label.backgroundColor = [UIColor clearColor];
	playerObject1Label.textColor = [UIColor whiteColor];
	playerObject1Label.textAlignment = UITextAlignmentLeft;
	
	UILabel *playerObject2Label = [[UILabel alloc] initWithFrame:CGRectMake(20, 400, 300, 31)];
	playerObject2Label.text = [NSString stringWithFormat:@"PlayerObject2: %p", PlayerObject2];
	playerObject2Label.backgroundColor = [UIColor clearColor];
	playerObject2Label.textColor = [UIColor whiteColor];
	playerObject2Label.textAlignment = UITextAlignmentLeft;
	
	setToZero = [[UISwitch alloc] initWithFrame:CGRectMake(250, 500, 0, 0)];
	[setToZero setOn:NO animated:YES];
	[setToZero addTarget:self action:@selector(magZero) forControlEvents:UIControlEventValueChanged];

	[uiview.view addSubview:cheatMenu];
	[cheatMenu addSubview:weaponMagSwitch];
	[cheatMenu addSubview:weaponMagLabel];
	[cheatMenu addSubview:carHealthSwitch];
	[cheatMenu addSubview:carHealthLabel];
	[cheatMenu addSubview:ammoSwitch];
	[cheatMenu addSubview:ammoLabel];
	[cheatMenu addSubview:healthSwitch];
	[cheatMenu addSubview:healthLabel];
	[cheatMenu addSubview:damageSwitch];
	[cheatMenu addSubview:damageLabel];
	[cheatMenu addSubview:playerObject0Label];
	[cheatMenu addSubview:playerObject1Label];
	[cheatMenu addSubview:playerObject2Label];
	[cheatMenu addSubview:setToZero];
	[cheatMenu addSubview:exitButton];
	[weaponMagSwitch release];
	[weaponMagLabel release];
	[carHealthSwitch release];
	[carHealthLabel release];
	[ammoSwitch release];
	[ammoLabel release];
	[damageSwitch release];
	[damageLabel release];
	[healthSwitch release];
	[healthLabel release];
	[setToZero release];

}

%new
	-(void)exitMenu{
	[cheatMenu removeFromSuperview];
}

%new
	-(void)showAlertWithTitle:(NSString *)title andMessage:(NSString *)message{
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title message:message delegate:self cancelButtonTitle:nil otherButtonTitles:@"OK", nil];
	[alert show];
	[alert release];
}

%new
-(void)weaponMagCheat{
	if(weaponMagSwitch.on){
		weaponMag = true;
		writeData(0x273960, "0123");
		writeData(0x4B1510, "0120"); //"0020" set to 0
	} else {
		weaponMag = false;
		writeData(0x273960, "013B");
		writeData(0x4B1510, "0138");
	}
}

%new
-(void)ammoCheat{
	if(ammoSwitch.on){
		ammo = true;
		writeData(0x4B2FC8, "6326");
		writeData(0x4B2F9C, "6326");
	} else {
		ammo = false;
		writeData(0x4B2FC8, "761A");
		writeData(0x4B2F9C, "013E");
	}
}

%new
-(void)carHealthCheat{
	if(carHealthSwitch.on){
		carHealth = true;
		writeData(0x5140BE, "00EB0A00");
	} else {
		carHealth = false;
		writeData(0x5140BE, "A0EB0A00");
	}
}

%new
-(void)healthCheat{
	if(healthSwitch.on){
		health = true;
		writeData(0x501D0A, "0021");
	} else {
		health = false;
		writeData(0x501D0A, "4146");
	}
}

%new
-(void)damageCheat{
	if(damageSwitch.on){
		damage = true;
	} else {
		damage = false;
	}
}
%new
-(void)magZero{
	writeData(0x4B1510, "0020");
}


- (void)showHTTPNotConnected:(id)fp8{

UILabel *label = MSHookIvar<UILabel*>(self, "m_titleLabel");

label.text = @"showHTTPNotConnected";
}

- (void)showHTTPErrorConnection:(id)fp8{

UILabel *label = MSHookIvar<UILabel*>(self, "m_titleLabel");

label.text = @"showHTTPErrorConnection";
}

%end

#ifdef _HOOKS_H
%ctor{
	//MSHookFunction((void *)0x4B2F55, (void *)replace_4B2F54, (void **)&sub_4B2F54);
	//MSHookFunction((void *)0x4A89F5, (void *)replace_4A89F4, (void **)&sub_4A89F4);
	MSHookFunction((void *)0x515039, (void *)replace_515038, (void **)&sub_515038);
	//MSHookFunction((void *)0x513F4D, (void *)replace_513F4C, (void **)&sub_513F4C);
	//MSHookFunction((void *)0x48A2F1, (void *)replace_48A2F0, (void **)&sub_48A2F0);
	MSHookFunction((void *)0x4B1339, (void *)replace_4B1338, (void **)&sub_4B1338);
	MSHookFunction((void *)0x47D723, (void *)hackDamage, (void **)&origDamage);
	MSHookFunction((void *)0x476241, (void *)playerObjectSubroutineHook, (void **)&origPlayerObjectSubroutine);
}
#endif