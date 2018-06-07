#pragma once

#include "Object.h"

class CUnit;

class CProjectile :public CObject
{
private:
	CProjectile();
	virtual ~CProjectile();
private:
	ProjectileTarget m_target;

	CUnit * m_pOwner;
	ProjectileData m_data;

	bool m_isMovable;
	Vec2 m_startPos;
	Vec2 m_destPos;
	Sprite* m_pSpr;
	float m_fMoved;
public:
	virtual bool init(unsigned int nIdx);

	static CProjectile* create(unsigned int idx);
public:
	void reset(CUnit* pOwner,Vec2 dir,Vec2 posZ,Vec2 posY,ProjectileTarget target);
private:
	virtual void execute(float fDelta);
	void movePosition();
	//void moveAirPosition(float fDelta);

	bool checkAttackBounding();
	bool checkAttkackOtherObject();
	bool checkAttackPlayer();

	Rect getAttackBox();
private:
	virtual void checkFrameEvent(Frame * pFrame);
	void pooledThis();
};