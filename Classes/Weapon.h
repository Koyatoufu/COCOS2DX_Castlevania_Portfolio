#pragma once

#include "Unit.h"
class CStageMap;

class CWeapon:public CObject
{
private:
	WeaponData m_wData;
	Sprite* m_pAttackBox;

	CUnit* m_pOwner;
	CStageMap* m_pStageMap;
private:
	CWeapon();
	virtual ~CWeapon();
public:
	virtual bool init(unsigned int nIdx);
	static CWeapon* create(unsigned int idx);
	//CREATE_FUNC_FILEPATH(CWeapon);
public:
	void playAnimation(std::string szAnimName, bool isLoop);
	void checkFrameEvent(Frame* pFrame);
public:
	Rect getAttackBoxBounding();

	void setUnitOwner(CUnit* pOwner);
};