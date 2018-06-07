#pragma once

#include "Defines.h"

#include "cocos2d.h"
#include "cocostudio\CocoStudio.h"

USING_NS_CC;

struct ObjHpData
{
	float fMaxHp;
	float fCurHp;
	float fMaxMp;
	float fCurMp;

	ObjHpData() :fMaxHp(-1.0f), fCurHp(-1.0f), fMaxMp(-1.0f), fCurMp(-1.0f) {};
	ObjHpData(float fHp, float fMp) :fMaxHp(fHp),fCurHp(fHp),fMaxMp(fMp),fCurMp(fMp) {};
};

struct UnitStatus 
{
	int nStr;
	int nCon;
	int nInt;
	int nReg;

	UnitStatus() :nStr(0), nCon(0), nInt(0), nReg(0){};
	UnitStatus(int nStr,int nCon,int nInt,int nReg) 
		:nStr(nStr), nCon(nCon), nInt(nInt), nReg(nReg) {};
};

struct CSNodeInfo
{
	Node *pNode;
	cocostudio::timeline::ActionTimeline *pTimeline;
	std::string szFilePath;
};

struct TileData 
{
	int nWidth;
	int nHeight;
	TileReg **ppArrReg;

	TileData():nWidth(0),nHeight(0),ppArrReg(0) {};
};

struct GravityData 
{
	bool isAir;
	float fGravity;
	float fYLimit;
	float fYFloatMin;

	GravityData() :isAir(false), fGravity(-1.0f), fYLimit(64.0f), fYFloatMin(100.0f) {};
	GravityData(bool isAir,float fGravity, float fYLimit, float fYFloatMin)
		:isAir(isAir),fGravity(fGravity),fYLimit(fYLimit),fYFloatMin(fYFloatMin) {};
};

struct AttackData
{
	AttackType type;
	HitOption option;
	int nAtk;
	unsigned int nEffIdx;
};

struct PlayerUnitData
{
	std::string filePath;
	std::string name;
	ObjHpData hpData;
	UnitStatus status;
	float fMoveSpeed;
	WeaponType weaponType;
	int nWeaponIdx;
	std::vector<int> subWeaponIdx;
};

struct EnemyUnitData
{
	std::string filePath;
	std::string name;
	EnemyType type;
	ObjHpData hpData;
	UnitStatus status;
	float fMoveSpeed;
	int nProjtileIdx;
};

struct ItemData
{
	std::string filePath;
	ItemType type;
	int nValue;
	int nSeIdx;
};

struct WeaponData
{
	std::string filePath;
	WeaponType type;
	int nAtk;
};

struct ProjectileData
{
	std::string filePath;
	std::string iconPath;
	ProjectileType type;
	AttackData atkData;
	float fCost;
	float fMoveSpeed;
	float fLiveTime;
	float fLength;
	float fAirForce;
	int nSEIdx;
};

struct EffectData
{
	std::string filePath;
	float fLiveTime;
	bool isGravity;
	int nSeIdx;
};

struct ObstacleData
{
	std::string filePath;
	ObjHpData hpData;
};

struct RoomData
{
	cocos2d::Size size;
	cocos2d::Vec2 position;
	bool isUseParallax;
	int nEnemyMin;
	int nObstacleMin;
	int nEnemyRand;
	int nObstacleRand;
};

struct StageData
{
	std::string name;
	std::string bgmPath;
	cocos2d::Size size;
	std::vector<RoomData> roomDatas;
	std::vector<int> enemyIdxes;
	int nObstacleIdx;
	PLUnitIdx unlockChar;
	std::vector<int> unlockStages;
};