#include "DataMgr.h"

CDataMgr * CDataMgr::m_pInst = nullptr;

CDataMgr::CDataMgr():m_pAtkData(0),m_pUserData(0),m_pStageData(0),m_pObjData(0)//,m_pStateData(0)
{
}

CDataMgr::~CDataMgr()
{
	if (m_pAtkData)
	{
		delete m_pAtkData;
		m_pAtkData = nullptr;
	}
	//if (m_pStateData)
	//{
	//	delete m_pStateData;
	//	m_pStateData = nullptr;
	//}
	if (m_pObjData)
	{
		delete m_pObjData;
		m_pObjData = nullptr;
	}
	if (m_pStageData)
	{
		delete m_pStageData;
		m_pStageData = nullptr;
	}
	if (m_pUserData)
	{
		m_pUserData->saveData();
		delete m_pUserData;
		m_pUserData = nullptr;
	}
}

CDataMgr * CDataMgr::getInst()
{
	if (!m_pInst)
	{
		m_pInst = new CDataMgr();
		m_pInst->initialize();
	}
	return m_pInst;
}

void CDataMgr::releaseInst()
{
	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = nullptr;
	}
}

void CDataMgr::initialize()
{
	FileUtils* pUtil = FileUtils::getInstance();

	m_pUserData = new CUserData();
	m_pAtkData = new CAttackData();
	m_pStageData = new CStageData();
	m_pObjData = new CObjectData();
	//m_pStateData = new CStateData();
}

CAttackData * CDataMgr::getAttackData()
{
	return m_pAtkData;
}

CUserData * CDataMgr::getUserData()
{
	return m_pUserData;
}

CStageData * CDataMgr::getStageData()
{
	return m_pStageData;
}

CObjectData * CDataMgr::getObjData()
{
	return m_pObjData;
}

//CStateData * CDataMgr::getStateData()
//{
	//return m_pStateData;
//}
