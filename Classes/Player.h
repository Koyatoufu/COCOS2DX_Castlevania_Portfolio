#pragma once

#include "Unit.h"
#include "Weapon.h"

class CPlayer : public CUnit 
{
protected:
	bool m_isAct;
	bool m_isHitable;
	bool m_isAtkable;
	int m_nAtkNum;
	CWeapon* m_pWeapon;
	//std::queue<> m_queueAtks;
protected:
	CPlayer();
	virtual ~CPlayer();
public:
	virtual bool init(unsigned int idx);

	static CPlayer* create(unsigned int idx);

	virtual void execute(float fDelta);
	virtual void setState(State idx);
	virtual void hpChange(float fHpSrc, AttackType type, HitOption option);

	void changeSubWeaponIdx(unsigned int nIdx);
protected:
	virtual void checkFrameEvent(Frame* pFrame);
	virtual void roomInsectCheck(Vec2 &pos);
public:
	bool getIsActive() { return m_isAct; }
	void setIsActive(bool isAct) { m_isAct = isAct; }
};