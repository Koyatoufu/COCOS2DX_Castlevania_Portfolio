#include "Room.h"
#include "StageMgr.h"

CRoom::CRoom():m_pFloor(0),m_pBg(0),m_tileData(),m_nEnemyCount(0)
{
}

CRoom::~CRoom()
{
	if (m_tileData.ppArrReg)
	{
		for (int i = 0; i < m_tileData.nHeight; i++)
		{
			delete[] m_tileData.ppArrReg[i];
		}

		delete[] m_tileData.ppArrReg;
	}
}

bool CRoom::init()
{
	if(!Node::init())
		return false;

	m_isHere = false;
	m_isGone = false;

	return true;
}

void CRoom::initRoom(std::string & szFloorName, Node *pBg)
{
	if (pBg != nullptr)
	{
		addChild(pBg);
	}

	m_pFloor = Sprite::create(szFloorName);
	m_pFloor->setAnchorPoint(Vec2(0.0, 0.0));
	addChild(m_pFloor);

	initfloorData();

	m_startPos = getPosition() + Vec2(FLOOR_TILE_SIZE, m_tileData.nHeight * 0.5f *FLOOR_TILE_SIZE);
	m_endRect.origin = getPosition()+ Vec2((m_tileData.nWidth - 1) * FLOOR_TILE_SIZE, 0.0f);
	m_endRect.size = Size(FLOOR_TILE_SIZE, FLOOR_TILE_SIZE * m_tileData.nHeight);

	m_SpawnPoint.origin = getPosition() + Vec2(FLOOR_TILE_SIZE * 0.25 * m_tileData.nWidth,0.0f);
	m_SpawnPoint.size = Size(m_tileData.nWidth * 0.5f * FLOOR_TILE_SIZE, m_tileData.nHeight * FLOOR_TILE_SIZE);

	setVisible(false);
}


void CRoom::initfloorData()
{
	m_tileData.nWidth = m_pFloor->getContentSize().width / FLOOR_TILE_SIZE;
	m_tileData.nHeight = m_pFloor->getContentSize().height / FLOOR_TILE_SIZE;

	m_tileData.ppArrReg = new TileReg*[m_tileData.nHeight];
	memset(m_tileData.ppArrReg, 0, sizeof(TileReg*)*m_tileData.nHeight);

	for (int i = 0; i < m_tileData.nHeight; i++)
	{
		m_tileData.ppArrReg[i] = new TileReg[m_tileData.nWidth];
		memset(m_tileData.ppArrReg[i], 0, sizeof(TileReg)*m_tileData.nWidth);
	}

	for (int i = 0; i < m_tileData.nHeight; i++)
	{
		for (int j = 0; j < m_tileData.nWidth; j++)
		{
			m_tileData.ppArrReg[i][j] = TileReg::NONE;
		}
	}
}

void CRoom::pushObjects(CObject * pObject)
{
	if (!pObject)
		return;

	m_curObjects.push_back(pObject);
}

void CRoom::resetRoom()
{
	if (!m_isGone)
	{
		setIsHere(true);

		for (unsigned int i = 0; i < m_curObjects.size(); i++)
		{
			m_curObjects[i]->setVisible(true);
		}
	}
}

void CRoom::clearRoom()
{
	setIsHere(false);

	if (m_curObjects.size() <= 0)
	{
		setIsGone(true);
		return;
	}

	for (unsigned int i = 0; i < m_curObjects.size(); i++)
	{
		m_curObjects[i]->setVisible(false);
	}
}

void CRoom::setTileReg(int nX, int nY,TileReg regData)
{
	if (nX >= m_tileData.nWidth)
		nX = m_tileData.nWidth - 1;
	if (nY >= m_tileData.nHeight)
		nY = m_tileData.nHeight - 1;

	m_tileData.ppArrReg[nY][nX] = regData;
}

void CRoom::addEnemyCount()
{
	m_nEnemyCount++;
}

void CRoom::minEnemyCount()
{
	if (m_nEnemyCount <= 0)
	{
		m_nEnemyCount = 0;
		return;
	}
	
	m_nEnemyCount--;
}

void CRoom::removeObjectFromList(CObject * pObject)
{
	if (m_curObjects.end() != find(m_curObjects.begin(), m_curObjects.end(), pObject))
	{
		m_curObjects.erase(find(m_curObjects.begin(), m_curObjects.end(), pObject));
		(CStageMgr::getInst()->getStageMap())->removeObjectFromList(pObject);
	}
}

TileReg CRoom::getTileReg(int nX, int nY) const
{
	//if (nX < 0)
	//	nX = 0;
	if (nX >= m_tileData.nWidth)
		nX = m_tileData.nWidth - 1;

	//if (nY < 0)
	//	nY = 0;
	if(nY>=m_tileData.nHeight)
		nY = m_tileData.nHeight - 1;

	TileReg regData = m_tileData.ppArrReg[nY][nX];

	return regData;
}

void CRoom::setIsHere(bool isHere)
{
	m_isHere = isHere;
	setVisible(m_isHere);
}

void CRoom::setIsGone(bool isGone)
{
	m_isGone = isGone;
}

Vec2 CRoom::getStartPos() const
{
	return m_startPos;
}

Rect CRoom::getEndRect() const
{
	return m_endRect;
}

Rect CRoom::getFloorBoundRect() const
{
	Rect rect;
	rect = m_pFloor->getBoundingBox();

	return rect;
}

Rect CRoom::getSpwanPoints() const
{
	return m_SpawnPoint;
}

bool CRoom::getIsClear() const
{
	return m_nEnemyCount <= 0 ? true : false;
}

bool CRoom::getIsHere() const
{
	return m_isHere;
}

bool CRoom::getIsGone() const
{
	return m_isGone;
}

std::vector<CObject*>& CRoom::getObjectsList()
{
	return m_curObjects;
}