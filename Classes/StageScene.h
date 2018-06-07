#pragma once

#include "PlayerMgr.h"
#include "StageMgr.h"

USING_NS_CC;

class CStageScene :public cocos2d::Scene 
{
private:
	CStageMap * m_pMap;
public:
	virtual ~CStageScene();
public:
	static cocos2d::Scene* createScene(unsigned int nIdx);

	virtual bool init(unsigned int nIdx);

	static CStageScene* create(unsigned int idx);
private:
	void update(float fDelta);
	void moveAreaCheck(CPlayer* pPlayer);
	void setViewPointCenter(Vec2 pos);
};