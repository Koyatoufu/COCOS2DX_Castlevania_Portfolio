#pragma once

#include "Includes.h"

#include "UIStage.h"
#include "UIResult.h"
#include "StageMap.h"

class CStageMgr 
{
private:
	static CStageMgr* m_pInst;

	CUIStage *m_pUI;
	CUIResult *m_pResultUI;
	CStageMap *m_pMap;

	int m_nCurStage;
private:
	CStageMgr();
	~CStageMgr();
public:
	static CStageMgr* getInst();
	static void releaseInst();
public:
	void initailize(unsigned int nIdx);

	void changeSubWeaponImg(int nIdx);
	void stageClearExecute();
	void gameoverExecute();
public:
	int getCurStageNumber();
	void setStageMap(CStageMap* pStage);
public:
	CUIStage * getStageUI();
	CUIResult * getResultUI();
	CStageMap * getStageMap();
};