#pragma once

#include "Includes.h"

class CObject :public cocos2d::Node 
{
protected:
	unsigned int m_nIdx;
	CSNodeInfo m_nodeInfo;
	Node * m_pYNode;
	Sprite * m_pZPic;

	Vec2 m_Dir;
	float m_fMoveSpeed;

	float m_fAirForce;
	GravityData m_GData;
	bool m_isGravityPause;

	ObjHpData m_hpData;
	HitOption m_bakOption;
	AttackType m_hitType;
	float m_fHitTime;
protected:
	CObject();
	virtual ~CObject();
public:
	virtual Rect getBoundZ() const;

	virtual CObject * cloneObj();
public:
	virtual bool init();

	CREATE_FUNC(CObject);

	void initHpData(float fHp,float fMp);
	virtual bool checkHitboxBound(Rect zRect, Rect yRect);
	virtual void hpChange(float fHpSrc, AttackType type, HitOption option);
	virtual void cloneDefaluts(CObject* pObj);
	virtual void execute(float fDelta);
protected:
	void hitoptionFunc(float fDelta);
	void gravityFunc(float fDelta);
	virtual void createNodeInfo(std::string szFilePath);
public:
	inline GravityData getGData() const;
	inline ObjHpData getHpData() const;
	inline Vec2 getDirection() const;
public:
	inline void setGravity(float fGravity);
	inline void setDirection(Vec2 dir);
};

Vec2 CObject::getDirection() const
{
	return m_Dir;
}

inline GravityData CObject::getGData() const
{
	return m_GData;
}

inline ObjHpData CObject::getHpData() const
{
	return m_hpData;
}

void CObject::setGravity(float fGravity)
{
	m_GData.fGravity = fGravity;
}
void CObject::setDirection(Vec2 dir)
{
	m_Dir = dir;
}
