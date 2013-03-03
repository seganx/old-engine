/********************************************************************
	created:	2011/07/12
	filename: 	GameTypes.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain all types used in the game
*********************************************************************/
#ifndef GUARD_GameTypes_HEADER_FILE
#define GUARD_GameTypes_HEADER_FILE

#include "ImportEngine.h"

#define _TEST_	1		//	test for games

#define CAMERA_FAR	6000.0f


#define MISSION_
#define MISSION_GOTO_NODE		0x00000001		//  goto specified node. data 0 & 1 most be pointer to scene nodes
#define MISSION_GOTO_POSITION	0x00000002		//  goto specified position. use no path nodes and go directly
#define MISSION_KILL_ENEMY		0x00000004		//  kill any enemies in the way
#define MISSION_SUICIDE			0x00000008		//  suicide !!!

#define	PARTY_TOWER				0x00000001
#define	PARTY_ENEMY				0x00000002
#define	PARTY_ALLY				0x00000004
#define	PARTY_PROJECTILE		0x00000008


#define	HEALTH_SELL				-99999			//	tower has been sold

#define EXCEL_CHAR_SEPERATOR	L" , "


typedef void (*Callback_Draw_Loading)( int count, int index, const WCHAR* state, const WCHAR* name );

enum MouseState
{
	MS_Null = 0,
	MS_ShowCreateTower,
	MS_CreateTower,
	MS_CreateHotzone,
	MS_CreateTrap,
	MS_MobileCamera,
	MS_FreeCamera,
	MS_ManualTower
};

enum EntityState
{
	ES_IDLE = 0,
	ES_WALK,
	ES_DIE,
	ES_DEAD,
	ES_SPELLING,
};

enum ProjectileType
{
	GPT_NULL = 0,
	GPT_BULLET,				//  a type of machine gun
	GPT_BOMB,				//  bomb
	GPT_CLUSTERBOMB,		//  cluster bomb
	GPT_MISSILE,			//  cluster bomb
};

enum TaskType
{
	GTT_NULL = 0,
	GTT_GOTO_NODE,			//	task to go to specified node
	GTT_GOTO_POSITION,		//	task to go to specified position
	GTT_ATTACK_WEAPON		//	attack to specified entity with weapon
};

enum MoveType
{
	GMT_NULL = 0,
	GMT_AIR,
	GMT_GROUND,
	GMT_BOTH
};

enum WeaponType
{
	GWT_NULL = 0,
	GWT_MACHINEGUN,
	GWT_SNOWER,
	GWT_MORTAR,
	GWT_GROUNDLAVA,
	GWT_FLIGHTER,
	GWT_LAUNCHER
};

enum GameMessageType
{
	GMT_BLANK = 0,			//  no message
	GMT_LEVEL_LOAD,			//	say to all objects that level is loading
	GMT_LEVEL_CLEAR,		//	say to all objects that level is clearing
	GMT_GAME_START,			//  say to all objects that game is starting
	GMT_GAME_END,			//  say to all objects that game is ending
	GMT_GAME_RESETING,		//  say to all objects that game is going to reset
	GMT_GAME_RESET,			//  say to all objects that game is reseting to reset themselves
	GMT_GAME_PAUSED,		//  say to all objects that game is paused
	GMT_WAVE_STARTED,		//	mechanic says that first wave has been started
	GMT_WAVE_FINISHED,		//	mechanic says that all waves have been finished
	GMT_I_INITIALIZED,		//  the entity is initialized. data will be pointer to entity
	GMT_I_FINALIZED,		//  the entity is finalized. data will be pointer to entity
	GMT_I_BORN,				//  the entity born. data will be pointer to new entity
	GMT_I_DIED,				//  the entity died. data will be pointer to death entity
	GMT_LEAVE_ME,			//	notify that cut the reference to this entity. data should be pointer to the entity
	GMT_GOTO,				//  request entity to go to the specified position. data will be pointer to a mission structure
	GMT_FIRE,				//  order to entity to fire to a position or some one
	GMT_DAMAGE,				//  send a damage to the entity. data will be pointer to msgDamage structure
	GMT_GOLDEN_TOWER,		//	notify to entity to make them golden. data should be pointer to float value describe the golden time
	GMT_SCREEN_RESIZED,		//	notify that the screen has been resized
};


struct Mission
{
	enum { MS_ACTIVE, MS_INACTIVE, MS_COMPLETE, MS_FAILED };

	UINT	status;		//  status of this mission
	UINT	flag;		//  use flags 'MISSION_'
	void*	data0;		//  additional data structure
	void*	data1;		//  additional data structure
	float3	pos;		//  data will use depend on flag
	float	time;		//  max time of the mission

	Mission(void): status(MS_ACTIVE), flag(0), data0(0), data1(0), pos(0,0,0), time(0) {}
};

struct prpHealth
{
	int		level[8];
	int		imax;
	int		icur;
	float	deleteTime;		// after this time in seconds entity will be deleted
	float	deathTime;		// death time will valued by animator

	prpHealth(void): imax(0), icur(0), deleteTime(6.0f), deathTime(0) { ZeroMemory(level, sizeof(level)); }
	void SetMax(int f){ imax = f; if (imax<icur) icur = imax; }
	void SetCur(int f){ icur = f; if (imax<icur) icur = imax; }
};

struct prpMove
{
	MoveType	type;
	
	float		moveSpeed;
	float		animSpeed;
	float		moveScale;
	float		animScale;

	bool		walk;

	prpMove(void): type(GMT_BOTH), moveSpeed(0), animSpeed(0), moveScale(1), animScale(1), walk(true) {}
};

struct prpAttack
{
	MoveType				targetType;
	float					physicalDamage;
	float					physicalArmor;
	float					electricalDamage;
	float					electricalArmor;
	float					stunValue;
	float					stunTime;
	float					splashRadius;
	float					goldenValue;
	float					rate;
	float					maxRange;
	float					minRange;
	int						killPeople;

	int						actionCount;
	float					actionTime;
	float					coolTime;
	float					chance;

	WCHAR					bullet[64];
	class Projectile*		projectile;

	prpAttack(void)
		: targetType(GMT_BOTH)
		, physicalDamage(0)
		, physicalArmor(0)
		, electricalDamage(0)
		, electricalArmor (0)
		, stunValue(0)
		, stunTime(0)
		, splashRadius(0)
		, goldenValue(1.0f)
		, rate(0)
		, maxRange(0)
		, minRange(0)
		, killPeople(0)
		, actionCount(0)
		, actionTime(0)
		, coolTime(0)
		, chance(0)
		, projectile(0) 
	{
		bullet[0]=0;
	}
};

struct msgFire
{
	float3	pos;
	void*	entity;
	bool	fired;			//  respond by entity to say that he fired

	msgFire(const float3& _pos, void* _entity): pos(_pos), entity(_entity), fired(false) {}
};


struct msgDamage
{
	float			physicalDamage;
	float			physicalArmor;
	float			electricalDamage;
	float			electricalArmor;
	float			stunValue;
	float			stunTime;
	class Entity*	sender;
	int				tag;

	msgDamage(
		const float physicaldamage,
		const float physicalarmor,
		const float electricaldamage,
		const float electricalarmor,
		const float stunvalue,
		const float stuntime,
		class Entity* _sender,
		const int _tag = 0
		)
		: physicalDamage( physicaldamage )
		, physicalArmor( physicalarmor )
		, electricalDamage( electricaldamage )
		, electricalArmor ( electricalarmor )
		, stunValue( stunvalue )
		, stunTime( stuntime )
		, sender( _sender )
		, tag(_tag)
	{}
};

struct PlayerProfile
{
	//	version 1
	WCHAR			name[32];
	int				achievements[15];
	int				upgrades[44];
	int				stars[10];
	int				level;
	int				people;

	//	version 2
	int				level_selected;
	int				level_played;

	//	version 3
	int				curDifficulty;
	int				difficulty[10];

	// version 4
	int				curGameMode;
	int				gameMode[10];

	PlayerProfile( void ){
		Reset();
	}

	void Reset( void ){
		ZeroMemory( this, sizeof(PlayerProfile) );
		level = 1;
		level_selected = 1;
		people = 100;
	}

	int GetNumStars( void )
	{
		int c = 0;
		for ( int i=0; i<10; i++ )
			c += stars[i];
		return c;
	}
};

struct Achievement
{
	WCHAR				name[32];
	WCHAR				desc[128];
	WCHAR				tips[128];
	WCHAR				icon[32];
	int					value;
	int					range;


	Achievement( void ){ ZeroMemory( this, sizeof(Achievement) ); }
	void Initialize( const WCHAR* cname, const WCHAR* cdesc, const WCHAR* ctips, const WCHAR* cicon, int irange );
	void AddValue( int val = 1 );
	bool IsUnlocked( void );
	void Finalize( void );
};

struct Upgrades
{
	union
	{
		struct {
			float	machinegun_discharger_power		;
			float	machinegun_discharger_count		;
			float	machinegun_range				;
			float	machinegun_firerate				;
			float	machinegun_damage				;
			float	snower_airback_chance			;
			float	snower_airback_time				;
			float	snower_airback_cooltime			;
			float	snower_range					;
			float	snower_firerate					;
			float	snower_freeze					;
			float	mortar_ability_chance			;
			float	mortar_splash					;
			float	mortar_ability_count			;
			float	mortar_ability_range			;
			float	mortar_range					;
			float	mortar_damage					;
			float	groundlava_headshot_cooltime	;
			float	groundlava_headshot_chance		;
			float	groundlava_headshot_time		;
			float	groundlava_range				;
			float	groundlava_firerate				;
			float	groundlava_damage				;
			float	flighter_crazy_chance			;
			float	flighter_crazy_cooltime			;
			float	flighter_crazy_range			;
			float	flighter_crazy_time				;
			float	flighter_damage					;
			float	general_gold_income				;
			float	general_sell_income				;
			float	general_people_resistance		;
			float	general_time_keeper				;
			float	deathrain_cooltime				;
			float	deathrain_count					;
			float	deathrain_range					;
			float	deathrain_splash				;
			float	trap_cooltime					;
			float	trap_damage						;
			float	trap_count						;
			float	goldentower_cooltime			;
			float	goldentower_time				;
			float	emp_cooltime					;
			float	emp_freeze_power				;
			float	emp_freeze_time					;
		};

		float value[44];
	};

	float	defaults[44];
	int		unlock[44];

	typedef WCHAR upstr[512];
	upstr	name[44];
	upstr	desc[44];

	Upgrades( void ){ ZeroMemory( this, sizeof(Upgrades) ); }
	void LoadDefaults( void );
	void Reset( void );
};


#endif	//	GUARD_GameTypes_HEADER_FILE