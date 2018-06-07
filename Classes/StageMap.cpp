#include "StageMap.h"

#include "ObjectPool.h"
#include "DataMgr.h"
#include "EnemyRunner.h"
#include "EnemyShooter.h"
#include "Boss.h"
#include "PlayerMgr.h"
#include "Obstacle.h"

bool CStageMap::init(unsigned int nIdx)
{
	if(!Node::init())
		return false;

	m_nStageIdx = nIdx;
	m_data = CDataMgr::getInst()->getStageData()->getStageData(nIdx);

	m_nLastRIdx = m_data.roomDatas.size() -1 ;
	m_nCurRIdx = 0;

	initSpace();

	return true;
}

CStageMap * CStageMap::create(unsigned int idx)
{
	CStageMap *pRet = new(std::nothrow) CStageMap();
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

void CStageMap::initSpace()
{
	setContentSize(m_data.size);

	createRoom();
	generateRooms();

	return;
}

void CStageMap::createRoom()
{
	m_Rooms.reserve(m_data.roomDatas.capacity());
	m_Rooms.resize(m_data.roomDatas.size());

	for (unsigned int i = 0; i < m_data.roomDatas.size(); i++)
	{
		m_Rooms[i] = CRoom::create();
		m_Rooms[i]->setPosition(m_data.roomDatas[i].position);
		m_Rooms[i]->setContentSize(m_data.roomDatas[i].size);
		addChild(m_Rooms[i]);
	}
}
void CStageMap::generateRooms()
{

	std::string szTmp = "";
	Node* pNode = nullptr;
	Sprite * pBg;
	Sprite * pWall;

	m_nLastRIdx = m_Rooms.size() - 1;

	srand(time(nullptr));

	for (int i = 0; i < (int)m_Rooms.size(); i++)
	{
		if (m_data.roomDatas[i].isUseParallax)
		{
			auto pParallax = ParallaxNode::create();
			pParallax->setAnchorPoint(Vec2::ZERO);
			pParallax->retain();

			pBg = Sprite::create(StringUtils::format("stage/%02d/bg_%02d.png",m_nStageIdx,i));
			pBg->setAnchorPoint(Vec2::ZERO);
			pParallax->addChild(pBg, 0, Vec2(0.1f, 0.0f), Vec2::ZERO);

			pWall = Sprite::create(StringUtils::format("stage/00/wall_%02d.png", i));
			pWall->setAnchorPoint(Vec2::ZERO);
			pParallax->addChild(pWall, 0, Vec2(0.5f, 0.0f), Vec2::ZERO);

			pNode = pParallax;
		}
		else
		{
			pNode = Node::create();
			pNode->setAnchorPoint(Vec2::ZERO);

			pBg = Sprite::create(StringUtils::format("stage/%02d/bg_%02d.png", m_nStageIdx, i));
			pBg->setAnchorPoint(Vec2::ZERO);
			pNode->addChild(pBg);

			pWall = Sprite::create(StringUtils::format("stage/00/wall_%02d.png", i));
			pWall->setAnchorPoint(Vec2::ZERO);

			pNode->addChild(pWall);
		}

		szTmp = StringUtils::format("stage/00/floor_%02d.png",i);

		m_Rooms[i]->initRoom(szTmp,pNode);

		createObstacle(m_Rooms[i],i);
		createEnemy(m_Rooms[i],i);
	}
}

std::vector<CObject*>& CStageMap::getObjectList()
{
	return m_ObjectList;
}

void CStageMap::createObstacle(CRoom * pRoom,int nIdx)
{
	int nRandX=0, nRandY=0;
	int nRandRegX = 0, nRandRegY = 0;

	int nCount = 0;
	
	int nCountMax = 0;
	if (m_data.roomDatas[nIdx].nObstacleRand)
		nCountMax = rand() % m_data.roomDatas[nIdx].nObstacleRand + m_data.roomDatas[nIdx].nObstacleMin;

	while (nCount <nCountMax)
	{
		nRandX = rand() % (int)pRoom->getSpwanPoints().size.width;
		nRandY = rand() % (int)pRoom->getSpwanPoints().size.height;

		if (!roomTileCheck(pRoom, nRandX, nRandY, nRandRegX, nRandRegY))
			continue;

		CObstacle *pObstacle = (CObstacle*)(CObjectPool::getInst()->getObject(ObjectType::OBSTACLE,m_data.nObstacleIdx));
		if (!pObstacle)
		{
			nCount++;
			continue;
		}

		pObstacle->setPosition(nRandX, nRandY);
		pObstacle->initHereRoom(pRoom, nRandRegX, nRandRegY);
		addChild(pObstacle);
		m_ObjectList.push_back(pObstacle);
		pRoom->pushObjects(pObstacle);

		nCount++;

	}
}

void CStageMap::createEnemy(CRoom * pRoom,int nIdx)
{
	CEnemy * pEnemy = nullptr;

	if (nIdx < (int)m_Rooms.size() - 1)
	{
		int nRandX = 0.0f, nRandY = 0.0f;
		int nRandRegX = 0, nRandRegY = 0;
		int nRandType = 0;

		int nCount = 0;
		int nCountMax = 0;
		if(m_data.roomDatas[nIdx].nEnemyRand)
			nCountMax = rand() % m_data.roomDatas[nIdx].nEnemyRand + m_data.roomDatas[nIdx].nEnemyMin;

		while(nCount<nCountMax)
		{
			nRandX = rand() % (int)pRoom->getSpwanPoints().size.width;
			nRandY = rand() % (int)pRoom->getSpwanPoints().size.height;

			if (!roomTileCheck(pRoom,nRandX,nRandY,nRandRegX,nRandRegY))
				continue;

			nRandType = rand() % (int)EnemyType::BOSS;
			pEnemy = (CEnemy*)CObjectPool::getInst()->getObject(ObjectType::UNIT, m_data.enemyIdxes[nRandType]);

			if (!pEnemy)
				continue;
			pEnemy->setPosition(nRandX, nRandY);
			this->addChild(pEnemy);
			pEnemy->initHereRoom(pRoom);
			pEnemy->setDirection(Vec2(-1.0f, 0.0f));

			pEnemy->setVisible(false);
			pRoom->pushObjects(pEnemy);
			m_ObjectList.push_back(pEnemy);
			pRoom->addEnemyCount();

			nCount++;
		}
	}
	else 
	{
		pEnemy = (CEnemy*)CObjectPool::getInst()->getObject(ObjectType::UNIT, m_data.enemyIdxes[3]);
		pEnemy->setPosition(pRoom->getSpwanPoints().getMidX(),pRoom->getSpwanPoints().getMidY());	
		addChild(pEnemy);
		pEnemy->initHereRoom(pRoom);
		pEnemy->setDirection(Vec2(1.0f, 0.0f));
		pEnemy->setVisible(false);
		pRoom->pushObjects(pEnemy);
		m_ObjectList.push_back(pEnemy);
	}
}

void CStageMap::recaculateObjectsZOrder(float fDelta)
{
	for (unsigned int i = 0; i < m_ObjectList.size(); i++)
	{
		float fPosY = m_ObjectList[i]->getPositionY();
		int nNewOrder = (int)((getContentSize().height)- fPosY);
		if (nNewOrder < 0)
			nNewOrder = 0;

		m_ObjectList[i]->setLocalZOrder(nNewOrder);
	}
}

bool CStageMap::roomTileCheck(CRoom * pRoom, int & nX, int & nY, int & nRegX, int & nRegY)
{
	nRegX = nX / FLOOR_TILE_SIZE;
	nRegY = nY / FLOOR_TILE_SIZE;

	nRegX += (pRoom->getSpwanPoints().getMinX() - pRoom->getPositionX()) / FLOOR_TILE_SIZE;
	nRegY += (pRoom->getSpwanPoints().getMinY() - pRoom->getPositionY()) / FLOOR_TILE_SIZE;

	if (pRoom->getTileReg(nRegX, nRegY) == TileReg::BLOCK)
		return false;

	nX += pRoom->getSpwanPoints().getMinX();
	nY += pRoom->getSpwanPoints().getMinY();

	return true;
}

CRoom * CStageMap::getCurrentRoom()
{
	if(m_Rooms.size() <= 0)
		return nullptr;
	return m_Rooms[m_nCurRIdx];
}

void CStageMap::setCurrentRoomIdx(int nIdx)
{
	m_nCurRIdx = nIdx;
	if (m_nCurRIdx >= m_nLastRIdx)
		m_nCurRIdx = m_nLastRIdx;
}

int CStageMap::getCurrentRoomIdx()
{
	return m_nCurRIdx;
}

int CStageMap::getLastRoomIdx()
{
	return m_nLastRIdx;
}

void CStageMap::update(float fDelta)
{
	for (unsigned int i = 0; i < m_ObjectList.size(); i++)
	{
		m_ObjectList[i]->execute(fDelta);
	}
	recaculateObjectsZOrder(fDelta);
}

void CStageMap::addObjectFromList(CObject * pObject)
{
	if (m_ObjectList.end() != std::find(m_ObjectList.begin(), m_ObjectList.end(), pObject))
		return;

	m_ObjectList.push_back(pObject);
}

void CStageMap::removeObjectFromList(CObject * pObject)
{
	if (m_ObjectList.end() != std::find(m_ObjectList.begin(), m_ObjectList.end(), pObject))
	{
		m_ObjectList.erase(std::find(m_ObjectList.begin(), m_ObjectList.end(), pObject));
	}
}

