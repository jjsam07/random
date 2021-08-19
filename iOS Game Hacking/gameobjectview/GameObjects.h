typedef enum {
	PistolAmmo,
	AK47Ammo,
	CrossbowAmmo,
	ShotgunAmmo,
	SMGAmmo,
	RPGAmmo,
	GrenadeAmmo,
	MolotovAmmo,
	SniperAmmo,
	UNKNOWN_UNUSEDAmmo,
	FlamethrowerAmmo,
	GatlingAmmo,
	GrenadeLauncherAmmo
} AmmoTypes;

char *weapon_name[] = {
	"UNUSED",
	"Bare Knuckles",
	"Baseball Bat",
	"Pistol",
	"Gatling",
	"Dual Pistol",
	"Shotgun",
	"SMG",
	"Dual SMGs",
	"AK-47",
	"Flamethrower",
	"RPG",
	"Grenade",
	"Molotov",
	"Sniper",
	"UNKNOWN_UNUSEDWeapon",
	"Golden Shotgun",
	"Golden Gatling",
	"Golden SMG",
	"Golden AK-47",
	"Golden Flamethrower",
	"Golden RPG",
	"Platinum AK-47",
	"Platinum SMG",
	"Platinum Gatling",
	"Flaming Crossbow",
	"Grenade Launcher",
	"Mannequin Leg"
};

typedef enum {
	BareKnuckles = 1,
	BaseballBat,		//2
	Pistol,				//3
	Gatling,			//4
	DualPistol,			//5
	Shotgun,			//6
	SMG,				//7
	DualSMGs,			//8
	AK47,				//9
	Flamethrower,		//10
	RPG,				//11
	Grenade,			//12
	Molotov,			//13
	Sniper,				//14
	UNKNOWN_UNUSEDWeapon,//15
	GoldShotgun,		//16
	GoldGatling,		//17
	GoldSMG,			//18
	GoldAK47,			//19
	GoldFlamethrower,	//20
	GoldRPG,			//21
	PlatinumAK47,		//22
	PlatinumSMG,		//23
	PlatinumGatling,	//24
	FlamingCrossbow,	//25
	GrenadeLauncher,	//26
	MannequinLeg		//27
} WeaponTypes;

typedef struct {
	char padding0[0x3C];
	int weapon_type;
	char padding1[8];
	int weapon_clip;
	char padding2[12];
	int damage;
	char padding3[0x8];
	int ammo_type;
	char padding4[0x44];
} Weapon;

typedef struct {
	Weapon *weapon[28];
	int weapon_ammo[13];
	int weapon_type;
	int zero;
} WeaponInventory;

typedef struct {
	char padding0[0x40];	//0x0
	float x;				//0x40
	float y;				//0x44
	float z;				//0x48
} PlayerPosition;

typedef struct {
	char padding0[0x58C];
	int health;
} Character;

typedef struct {
	void			*vtable0;					//0x0
	void			*vtable1;					//0x4
	char			padding0[0xC];	//padding	//0x8
	PlayerPosition	*position0;					//0x14
	char			padding1[0x2E0];//padding	//0x18
	PlayerPosition	*position1; 				//0x2F8
	char			padding2[0x28];	//padding	//0x2FC
	float			playerVelocity0;			//0x324
	char			padding3[0x1C];	//padding	//0x328
	float			playerVelocity1;			//0x344
	char			padding4[0x170];//padding	//0x348
	bool			isPlayerTargeting;			//0x4B8
	char			padding5[0x60];	//padding	//0x4BC
	float			cameraX;					//0x51C
	float			cameraY;					//0x520
	float			cameraZ;					//0x524
	char			padding6[0x64]; //padding	//0x528
	int				maxHealth; 					//0x58C
	int				health; 					//0x590
	char			padding7[0x8];	//padding	//0x594
	bool			isPlayerMoving;				//0x59C
	char			padding8[0xC8]; //padding	//0x5A0
	Character		*Char;						//0x668 //Target's Object
	char			padding9[0xCC];	//padding	//0x66C
	float			dpadDirection;				//0x738
	bool			canSprint;					//0x73C
	char			padding10[0x88];//padding	//0x740
	int				distanceTravelledOnFoot;	//0x7C8
	int				distanceTravelledOnVehicle;	//0x7CC
	char			padding11[0x14];//padding	//0x7D0
	float			playerX;					//0x7E4
	float			playerY;					//0x7E8
	float			playerZ;					//0x7EC
	char			padding12[0x24];//padding	//0x7F0
	WeaponInventory	*Inventory;					//0x814
} Player;