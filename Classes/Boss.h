#pragma once

#include "Enemy.h"

class CBoss :public CEnemy
{
protected:
	CBoss();
	virtual ~CBoss();
public:
	virtual bool init(unsigned int nIdx);

	static CBoss* create(unsigned int idx);

	virtual void execute(float fDelta);
	virtual void setState(State idx);
	virtual void hpChange(float fHpSrc, AttackType type,HitOption option);
protected:
	virtual Rect getAttackBoxBounding() override;
	virtual void checkFrameEvent(Frame * pFrame);
};
