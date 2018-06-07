#pragma once

#include "Unit.h"
#include "Room.h"
#include "ui\UILoadingBar.h"

class CEnemy :public CUnit
{
protected:
	CRoom * m_pRoom;
	EnemyType m_type;
	int m_nEnemyIdx;
	bool m_isActing;
	float m_fWaitTime;
	Node* m_pAttackBox;

	ui::LoadingBar* m_pHpbar;
protected:
	CEnemy();
	virtual ~CEnemy();
public:
	virtual bool init(unsigned int nIdx);
	virtual void setState(State idx);
	virtual void hpChange(float fHpSrc, AttackType type,HitOption option);
protected:
	virtual void roomInsectCheck(Vec2 &pos);
public:
	void initHereRoom(CRoom* pRoom);
	inline CRoom* getRoom() const;
	inline EnemyType getType() const;
protected:
	virtual Rect getAttackBoxBounding();
protected:
	virtual void checkFrameEvent(Frame* pFrame);
	void pooledThis();
};

inline CRoom * CEnemy::getRoom() const
{
	return m_pRoom;
}

inline EnemyType CEnemy::getType() const
{
	return m_type;
}