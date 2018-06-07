#include "StageScene.h"
#include "DataMgr.h"
#include "ObjectPool.h"
#include "SoundMgr.h"

CStageScene::~CStageScene()
{
	CPlayerMgr::releaseInst();
	CStageMgr::releaseInst();
	CObjectPool::releaseInst();
}

cocos2d::Scene * CStageScene::createScene(unsigned int nIdx)
{
	return CStageScene::create(nIdx);
}

bool CStageScene::init(unsigned int nIdx)
{
	if (!Scene::init()) 
	{
		return false;
	}

	CObjectPool::getInst()->createPool(nIdx);
	auto pMgr = CStageMgr::getInst();
	pMgr->initailize(nIdx);
	CPlayerMgr::getInst();

	m_pMap = CStageMap::create(nIdx);
	addChild(m_pMap);
	pMgr->setStageMap(m_pMap);

	StageData data = CDataMgr::getInst()->getStageData()->getStageData(nIdx);
	CSoundMgr::getInst()->playBgm(data.bgmPath, true);

	auto pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();
	pPlayer->setPosition(m_pMap->getCurrentRoom()->getStartPos());
	pPlayer->changeSubWeaponIdx(0);

	m_pMap->addChild(pPlayer);
	m_pMap->addObjectFromList(pPlayer);

	CRoom* pRoom = m_pMap->getCurrentRoom();
	m_pMap->setPosition(-pRoom->getPosition());
	pRoom->resetRoom();
	
	addChild(pMgr->getStageUI());
	addChild(pMgr->getResultUI());

	scheduleUpdate();

	return true;
}

CStageScene * CStageScene::create(unsigned int idx)
{
	CStageScene *pRet = new(std::nothrow) CStageScene();
	if (pRet && pRet->init(idx))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

void CStageScene::update(float fDelta)
{
	CStageMgr::getInst()->getStageUI()->update(fDelta);
	m_pMap->update(fDelta);

	CPlayer* pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();

	if (!pPlayer)
		return;

	moveAreaCheck(pPlayer);
	setViewPointCenter((pPlayer->getPosition()));
}

void CStageScene::moveAreaCheck(CPlayer* pPlayer)
{
	if (m_pMap->getCurrentRoomIdx() == m_pMap->getLastRoomIdx())
		return;

	if (m_pMap->getCurrentRoom()->getIsClear())
	{
		if (m_pMap->getCurrentRoom()->getEndRect().containsPoint(pPlayer->getPosition()))
		{
			CRoom* pBak = m_pMap->getCurrentRoom();
			CRoom* pNewRoom = nullptr;
			pBak->clearRoom();
			m_pMap->setCurrentRoomIdx(m_pMap->getCurrentRoomIdx() + 1);
			pNewRoom = m_pMap->getCurrentRoom();
			pNewRoom->resetRoom();
			pPlayer->setPosition(pNewRoom->getStartPos());
			if (m_pMap->getCurrentRoomIdx() == m_pMap->getLastRoomIdx())
			{
				CSoundMgr::getInst()->playBgm("sounds/bgm/boss.mp3", true);
			}
		}
	}
}


void CStageScene::setViewPointCenter(Vec2 pos)
{
	Size winSize = Director::getInstance()->getWinSize();

	float x = MAX(pos.x, winSize.width*0.5f + m_pMap->getCurrentRoom()->getPosition().x);
	float y = MAX(pos.y, winSize.height*0.5f + m_pMap->getCurrentRoom()->getPosition().y);

	x = MIN(x, (m_pMap->getCurrentRoom()->getContentSize().width + m_pMap->getCurrentRoom()->getPosition().x) - (winSize.width*0.5f));
	y = MIN(y, (m_pMap->getCurrentRoom()->getContentSize().height + m_pMap->getCurrentRoom()->getPosition().y) - (winSize.height*0.5f));

	Vec2 actualPos = Vec2(x, y);
	Vec2 centerOfView = Vec2(winSize.width*0.5f, winSize.height*0.5f);
	Vec2 viewPoint = centerOfView - actualPos;

	m_pMap->setPosition(viewPoint);
	
}