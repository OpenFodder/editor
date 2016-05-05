
struct sSpriteDef {
	uint16 mDirection;
	uint16 mIgnored;
	uint16 mX;
	uint16 mY;
	uint16 mSpriteID;
};

struct sSpriteSheet {
	uint16 field_0;
	uint16 field_2;
	int16 field_4;
	int16 field_6;
	int16 mColCount;
	int16 mRowCount;
	int16 field_C;
	int8 field_E;
	int8 field_F;
};

extern const sSpriteSheet* off_32C0C[];

enum eSprites {
	eSprite_Player							= 0,
	eSprite_Grenade							= 2,
	eSprite_ShadowSmall						= 3,
	eSprite_Null							= 4,
	eSprite_Enemy							= 5,
	eSprite_Bullet							= 6,
	eSprite_Snowman							= 16,
	eSprite_Explosion						= 12,
	eSprite_Shrub							= 13,
	eSprite_Tree							= 14,
	eSprite_BuildingRoof					= 15,
	eSprite_Shrub2							= 17,
	eSprite_Waterfall						= 18,
	eSprite_Bird2_Left						= 19,

	eSprite_BuildingDoor					= 20,
	eSprite_Player_Rank						= 21,
	eSprite_GroundHole						= 24,
	eSprite_BuildingDoor2					= 25,
	eSprite_Floating_Dead_Soldier			= 27,
	eSprite_Text_Complete					= 28,
	eSprite_Text_Mission					= 29,
	eSprite_Text_Phase						= 30,
	eSprite_Rocket							= 33,
	eSprite_Text_GameOver					= 34,
	eSprite_Shadow							= 35,
	eSprite_Enemy_Rocket					= 36,
	eSprite_GrenadeBox						= 37,
	eSprite_RocketBox						= 38,
	eSprite_Building_Explosion				= 39,
	eSprite_Helicopter_Grenade_Enemy			= 40,
	eSprite_Flashing_Light						= 41,
	eSprite_Helicopter_Grenade2_Enemy			= 42,
	eSprite_Helicopter_Missile_Enemy			= 43,
	eSprite_Helicopter_Homing_Enemy				= 44,
	eSprite_Missile								= 45,
	eSprite_MissileHoming						= 46,
	eSprite_Helicopter_Grenade2_Human			= 49,
	eSprite_Helicopter_Grenade_Human			= 50,
	eSprite_Helicopter_Missile_Human			= 51,
	eSprite_Helicopter_Homing_Human				= 52,
	eSprite_Mine								= 54,
	eSprite_Mine2								= 55,
	eSprite_Spike								= 56,
	eSprite_Text_Try							= 58,
	eSprite_Text_Again							= 59,

	eSprite_BoilingPot							= 60,
	eSprite_Indigenous							= 61,
	eSprite_Indigenous2							= 62,
	eSprite_VehicleNoGun_Human					= 63,		// Skidu
	eSprite_VehicleGun_Human					= 64,
	eSprite_Tank_Human							= 65,
	eSprite_Bird_Left							= 66,
	eSprite_Bird_Right							= 67,
	eSprite_Seal								= 68,
	eSprite_Tank_Enemy							= 69,

	eSprite_Indigenous_Spear					= 70,
	eSprite_Hostage								= 72,
	eSprite_Hostage_Rescue_Tent					= 73,
	eSprite_Door_Indigenous						= 74,
	eSprite_Door2_Indigenous					= 75,
	eSprite_Door_Indigenous_Spear				= 76,
	eSprite_Turret_Missile_Human				= 78,
	eSprite_Turret_Missile2_Human				= 79,

	eSprite_VehicleNoGun_Enemy					= 80,
	eSprite_VehicleGun_Enemy					= 81,
	eSprite_Vehicle_Unk_Enemy					= 82,
	eSprite_Indigenous_Invisible				= 83,
	eSprite_Turret_Missile_Enemy				= 84,
	eSprite_Turret_Missile2_Enemy				= 85,
	eSprite_BuildingDoor3						= 88,
	eSprite_Explosion2							= 89,

	eSprite_OpenCloseDoor						= 90,
	eSprite_Seal_Mine							= 91,
	eSprite_Spider_Mine							= 92,
	eSprite_Bonus_RankToGeneral					= 93,
	eSprite_Bonus_Rockets						= 94,
	eSprite_Player_Rocket						= 95,
	eSprite_Bonus_RocketsAndGeneral				= 96,
	eSprite_MissileHoming2						= 97,
	eSprite_Bonus_SquadGeneralRockets			= 98,
	eSprite_Helicopter_CallPad					= 99,

	eSprite_BuildingDoor_Reinforced				= 100,
	eSprite_Helicopter_Grenade2_Human_Called	= 101,
	eSprite_Helicopter_Grenade_Human_Called		= 102,
	eSprite_Helicopter_Missile_Human_Called		= 103,

	eSprite_Helicopter_Homing_Human_Called		= 104,
	eSprite_Turret_HomingMissile_Enemy			= 105,
	eSprite_Hostage_2							= 106,
	eSprite_Helicopter_Homing_Enemy2			= 107,
	eSprite_Computer_1							= 108,
	eSprite_Computer_2							= 109,
	eSprite_Computer_3							= 110,
};