#include "StageMgr.h"
#include "PlayerMgr.h"
#include "SoundMgr.h"

CStageMgr* CStageMgr::m_pInst = nullptr;

CStageMgr::CStageMgr():m_pUI(),m_pResultUI(),m_nCurStage(-1),m_pMap(0)
{
}

CStageMgr::~CStageMgr()
{
}


CStageMgr * CStageMgr::getInst()
{
	if (!m_pInst)
	{
		m_pInst = new CStageMgr();
	}

	return m_pInst;
}

void CStageMgr::releaseInst()
{
	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = nullptr;
	}
}

void CStageMgr::initailize(unsigned int nIdx)
{
	if (m_nCurStage != -1)
		return;

	m_nCurStage = nIdx;

	m_pUI = CUIStage::create();
	m_pUI->retain();

	m_pResultUI = CUIResult::create();
	m_pResultUI->retain();
}

void CStageMgr::changeSubWeaponImg(int nIdx)
{
	m_pUI->changeSubweaponImg((unsigned int)nIdx);
}

void CStageMgr::stageClearExecute()
{
	m_pUI->setVisible(false);
	m_pResultUI->playStageClear();
	CSoundMgr::getInst()->playBgm("sounds/bgm/clear.mp3", false);
}

void CStageMgr::gameoverExecute()
{
	if (CPlayerMgr::getInst()->getCurrentPlayer()&& !CPlayerMgr::getInst()->getCurrentPlayer()->getIsDead())
		return;

	m_pUI->setVisible(false);
	m_pResultUI->playGameOver();
	CSoundMgr::getInst()->playBgm("sounds/bgm/gameover.mp3", false);
}

int CStageMgr::getCurStageNumber()
{
	return m_nCurStage;
}

void CStageMgr::setStageMap(CStageMap * pStage)
{
	if (m_pMap)
		return;
	m_pMap = pStage;
}

CUIStage * CStageMgr::getStageUI()
{
	return m_pUI;
}

CUIResult * CStageMgr::getResultUI()
{
	return m_pResultUI;
}

CStageMap * CStageMgr::getStageMap()
{
	return m_pMap;
}
