#pragma once

#include "Object.h"

class CUnit :public CObject
{
protected:
	SkeletonNode * m_pSkeleton;
	UnitStatus m_unitStatus;
	State m_stateIdx;

	bool m_isJumpStart;
	bool m_isDead;
	Vec2 m_destPos;

	Sprite* m_pHpBack;
	ProgressTimer* m_pHpTimer;

	int m_nSubIdx;
protected:
	CUnit();
	virtual ~CUnit();
public:
	virtual bool init(std::string szFilePath);
	static CUnit* create(std::string szFilePath);
public:
	virtual CUnit *cloneObj();
protected:
	virtual void cloneDefaluts(CUnit* pObj);
protected:
	virtual void checkFrameEvent(Frame* pFrame);
	virtual void roomInsectCheck(Vec2 &pos);
public:
	virtual void setState(State idx);
	State getStateIdx() const;
	void setSkeltonPath(std::string szFilePath);
public:
	//if insert hitbox in check rect return true
	virtual bool checkHitboxBound(Rect zRect,Rect yRect);
	virtual void hpChange(float fHpSrc, AttackType type, HitOption option);
	void mpChange(float fCost);
protected:
	void shootProjectile(ProjectileTarget target);
	Rect getHitboxBounding();
	virtual void createNodeInfo(std::string szFilePath);
public:
	inline bool getIsDead() const;
	inline int getSubweaponIdx() const;
};

inline bool CUnit::getIsDead() const
{
	return m_isDead;
}

inline int CUnit::getSubweaponIdx() const
{
	return m_nSubIdx;
}
