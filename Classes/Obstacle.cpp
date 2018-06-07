#include "Obstacle.h"
#include "DataMgr.h"
#include "StageMgr.h"
#include "Item.h"
#include "ObjectPool.h"

#define OBSTACLE_BODY 70

CObstacle::CObstacle():CObject(),m_pSpr(0),m_pRoom(0)
{
}

CObstacle::~CObstacle()
{
}

bool CObstacle::init(unsigned int nIdx)
{
	if(!CObject::init())
		return false;

	m_nIdx = nIdx;
	auto data = CDataMgr::getInst()->getObjData()->getObstacleData(nIdx);
	
	initHpData(data.hpData.fMaxHp, data.hpData.fMaxMp);

	createNodeInfo(data.filePath);
	m_nodeInfo.pNode->setAnchorPoint(Vec2::ZERO);
	m_pSpr = (Sprite*)m_nodeInfo.pNode->getChildByTag(OBSTACLE_BODY);
	m_nodeInfo.pTimeline->play("idle",true);
	m_nodeInfo.pTimeline->setFrameEventCallFunc(CC_CALLBACK_1(CObstacle::checkFrameEvent,this));

	m_pZPic = Sprite::create("common/empty.png");
	m_pZPic->setContentSize(Size(m_pSpr->getContentSize().width, FLOOR_TILE_SIZE));
	m_pZPic->setAnchorPoint(Vec2::ZERO);
	addChild(m_pZPic);
	
	return true;
}

CObstacle * CObstacle::create(unsigned int idx)
{
	CObstacle *pRet = new(std::nothrow) CObstacle();
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

void CObstacle::execute(float fDelta)
{
	hitoptionFunc(fDelta);
}

void CObstacle::hpChange(float fHpSrc, AttackType type , HitOption option)
{
	if (!isVisible())
		return;
	if (m_hpData.fCurHp <= 0.0f)
		return;

	CObject::hpChange(fHpSrc, type, option);

	if (m_hpData.fCurHp <= 0.0f)
	{
		m_nodeInfo.pTimeline->play("dead",false);
	}
}

void CObstacle::initHereRoom(CRoom * pRoom, int nX, int nY)
{
	if (m_pRoom)
		return;

	m_pRoom = pRoom;
	m_nXReg = nX;
	m_nYReg = nY;

	int nXsize = round(m_pSpr->getContentSize().width / FLOOR_TILE_SIZE);
	
	for (int i = 0; i < nXsize; i++)
	{
		m_pRoom->setTileReg(nX + i, nY, TileReg::BLOCK);
	}
}

bool CObstacle::checkHitboxBound(Rect zRect, Rect yRect)
{
	Rect zBound = getBoundZ();
		
	zBound.origin.x = zRect.origin.x;

	if (zBound.intersectsRect(zRect))
	{
		Rect hitbox = m_pSpr->getBoundingBox();
		hitbox.origin = convertToWorldSpace(hitbox.origin);
		if (hitbox.intersectsRect(yRect))
		{
			log("Y in");

			return true;
		}
	}

	return false;
}

void CObstacle::checkFrameEvent(Frame * pFrame)
{
	EventFrame* pEvent = dynamic_cast<EventFrame*>(pFrame);
	if (!pEvent)
		return;

	std::string szEvt = pEvent->getEvent();

	if (szEvt.empty())
		return;

	if (szEvt.compare("dead")==0)
	{
		int nXsize = round(m_pSpr->getContentSize().width / FLOOR_TILE_SIZE);

		for (int i = m_nXReg; i < m_nXReg + nXsize; i++)
		{
			m_pRoom->setTileReg(i, m_nYReg, TileReg::NONE);
		}

		srand(time(nullptr));
		int nRate = rand() % 100;
		if (nRate >= 30)
		{
			unsigned int nRandIdx = rand() % (int)ItemType::ORB;
			auto pItem = (CItem*)CObjectPool::getInst()->getObject(ObjectType::ITEM,nRandIdx);
			Vec2 pos = getPosition();
			pos.x += FLOOR_TILE_SIZE;
			pItem->resetItem(pos);
		}

		m_pRoom->removeObjectFromList(this);
		CObjectPool::getInst()->PoolObject(this, ObjectType::PROJECTILE,m_nIdx);

		return;
	}
}
