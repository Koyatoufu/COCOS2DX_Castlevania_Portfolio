#pragma once

#define DPAD 201
#define DPAD_HANDLE 202
#define PAUSE_MENU 81
#define SELECT_IMAGE 56
#define DIR_NOR_LENGTH 1.1f
#define FLOOR_TILE_SIZE 32.0f
#define TAG_WEAPON 69

#define HITBOX 123
#define ATTACKBOX 456

#define ENEMY_DEFAULT_WAITTIME 0.5f
#define PROJECTILE_LIVETIME 5.0f

#define UNLOCKABLE_CHAR_MAX 4
#define UNLOCKABLE_STAGE_MAX 3

#define MESSAGE_TEXT 666
#define BTN_RETURN 555
#define CHAR_PORTRAIT 999

enum class QuitMenu 
{
	MENU = 164,
	YES = 165,
	NO = 166,
};

enum class TileReg
{
	NONE,
	BLOCK
};

enum class BtnStage
{
	ATK = 111,
	JUMP = 112,
	EVADE = 113,
	SUB = 114,
	SP = 115,
	CHANGE = 116,
	PAUSE = 117,
	RESUME = 118,
	MENU = 119
};

enum class StageBar
{
	CASE = 333,
	HPBAR = 301,
	MPBAR = 302
};

enum class BtnSel
{
	STAGE = 31,
	CHARINFO,
	ACHEIVE,
	OPTION,
};

enum class BtnSel_Idx
{
	STAGE = 0,
	CHARINFO,
	ACHIEVE,
	OPTION,
	MAX
};

enum class PLUnitIdx
{
	TREVOR = 0,
	SYPHA = 1,
	ALUCARD = 2,
	GRANT = 3,
	HECTOR = 4,
	ANGELA = 5,
	NONE = 6,
};

enum class WeaponType
{
	NONE,
	WHIP,
	STAFF,
	SWORD,
	DAGGER
};

enum class State
{
	IDLE = 0,
	MOVE,
	JUMP,
	ATTACK,
	SUBATTACK,
	HIT,
	DOWN,
	DEAD,
	EVADE,
	SKILL,
	NONE
};

enum class EnemyType 
{
	RUNNER,
	SHOOTER,
	BRAWER,
	BOSS,
	NONE
};

enum class ItemType
{
	HEAL,
	CHARGE,
	SUBWEAPONCHANGE,
	ORB,
	NONE
};

enum class ProjectileType
{
	NORMAL = 0,
	AIRUSE,
	RETURN,
	PUTTING,
	GROUNDRUNNING,
	BOME,
	NONE
};

enum class ProjectileTarget
{
	OTHER,
	PLAYER,
};

enum class AttackIdx
{
	Player,
	Enemy,
	Projectile
};

enum class AttackType
{
	NONE,
	FLOAT,
	PUSH,
};

enum class HitOption
{
	ONEFRAME,
	MULTYFRAME
};

enum class ObjectType
{
	UNIT,
	ITEM,
	PROJECTILE,
	EFFECT,
	OBSTACLE,
	MAX
};