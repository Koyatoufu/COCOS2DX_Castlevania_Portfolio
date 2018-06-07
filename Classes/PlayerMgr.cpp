#include "PlayerMgr.h"
#include "StageMgr.h"
#include "DataMgr.h"

CPlayerMgr * CPlayerMgr::m_pInst = nullptr;

CPlayerMgr::CPlayerMgr() :m_isInit(false), m_pCurPL(0), m_pResPL(0), m_isGameOver(false)
{
}

CPlayerMgr::~CPlayerMgr()
{
	if (m_pCurPL)
	{
		m_pCurPL->release();
	}
	if (m_pResPL)
	{
		m_pResPL->release();
	}
}

CPlayerMgr * CPlayerMgr::getInst()
{
	if (!m_pInst)
	{
		m_pInst = new CPlayerMgr();
		m_pInst->initialize();
	}

	return m_pInst;
}

bool CPlayerMgr::initialize()
{
	if (m_isInit)
		return false;

	CDataMgr *pDataMgr = CDataMgr::getInst();

	CUserData *pUserData = pDataMgr->getUserData();

	PLUnitIdx idx1 = pUserData->getUserPLIDX1();

	if (idx1 == PLUnitIdx::NONE)
		return false;

	m_pCurPL = CPlayer::create((unsigned int)idx1);
	m_pCurPL->retain();
	m_pCurPL->setIsActive(true);
	m_isInit = true;
	m_isGameOver = false;

	if (pUserData->getUserPLIDX2() == PLUnitIdx::NONE)
	{
		m_pResPL = nullptr;
		return true;
	}

	m_pResPL = CPlayer::create((unsigned int)pUserData->getUserPLIDX2());
	m_pResPL->retain();
	m_pResPL->setIsActive(false);

	return true;
}

bool CPlayerMgr::isPlayersAllDead()
{
	if (!m_pCurPL)
		return true;

	if (m_pCurPL->getIsDead())
	{
		if (!m_pResPL)
			return true;

		if (m_pResPL->getIsDead())
			return true;
	}

	return false;
}

void CPlayerMgr::changePlayer()
{
	if (!m_pResPL)
		return;

	if (m_pResPL->getIsDead())
		return;

	CPlayer* pTemp = m_pCurPL;
	m_pCurPL = m_pResPL;
	m_pResPL = pTemp;
}

void CPlayerMgr::deadAccept(CPlayer * pPlayer)
{
	if (m_pCurPL == pPlayer)
	{
		if (!m_pResPL || m_pResPL->getIsDead())
		{
			CStageMgr::getInst()->gameoverExecute();
			return;
		}

		m_pCurPL = m_pResPL;
		m_pResPL = pPlayer;
		return;
	}
}

void CPlayerMgr::releaseInst()
{
	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = nullptr;
	}
}