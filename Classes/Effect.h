#pragma once

#include "Object.h"

class CEffect:public CObject
{
protected:
	EffectData m_data;
	float m_fLiveTime;
	float m_fLimitTime;
protected:
	CEffect();
	virtual ~CEffect();
public:
	static CEffect* create(unsigned int idx);

	virtual void execute(float fDelta);
protected:
	virtual bool init(unsigned int nIdx);
public:
	void startEffects(Vec2 zPos,Vec2 yPos);
	void poolThis();
};