#pragma once

#include "Object.h"
class CRoom;

class CObstacle :public CObject
{
private:
	Sprite * m_pSpr;
	CRoom * m_pRoom;
	int m_nXReg;
	int m_nYReg;
	float m_fHitTime;
private:
	CObstacle();
	virtual ~CObstacle();
public:
	virtual bool init(unsigned int nIdx);
	static CObstacle* create(unsigned int idx);
	virtual void execute(float fDelta)override;
public:
	virtual void hpChange(float fHpSrc, AttackType type, HitOption option);
	void initHereRoom(CRoom *pRoom,int nX,int nY);
public:
	virtual bool checkHitboxBound(Rect zRect, Rect yRect);
protected:
	virtual void checkFrameEvent(Frame* pFrame);
};