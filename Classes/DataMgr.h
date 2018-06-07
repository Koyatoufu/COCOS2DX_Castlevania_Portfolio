#pragma once

#include "UserData.h"
#include "ObjectData.h"
#include "AttackData.h"
#include "StageData.h"
//#include "StateData.h"

class CDataMgr
{
private:
	static CDataMgr * m_pInst;

	CAttackData* m_pAtkData;
	CUserData* m_pUserData;
	CStageData* m_pStageData;
	CObjectData* m_pObjData;
	//CStateData* m_pStateData;
private:
	CDataMgr();
	~CDataMgr();
public:
	static CDataMgr* getInst();
	static void releaseInst();
public:
	void initialize();
	CAttackData* getAttackData();
	CUserData* getUserData();
	CStageData* getStageData();
	CObjectData* getObjData();
	//CStateData* getStateData();
};