#pragma once

#include "Object.h"

class CPlayer;

class CItem : public CObject
{
private:
	Sprite * m_pSpr;
	ItemData m_data;
	float m_fLiveTime;
protected:
	CItem();
	virtual ~CItem();
public:
	virtual bool init(unsigned int nIdx);

	static CItem* create(unsigned int idx);

	virtual void execute(float fDelta);
	void resetItem(Vec2 pos);
private:
	void useItem(CPlayer *pPlayer);
	Rect getItemRect();
	void pooledThis();
};