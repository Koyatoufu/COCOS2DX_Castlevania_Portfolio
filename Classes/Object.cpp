#include "Object.h"

CObject::CObject() :	Node(),m_nIdx(999), m_nodeInfo(), m_pYNode(0), m_pZPic(0), m_GData(), m_Dir(), m_hpData(),
					m_isGravityPause(false), m_fMoveSpeed(0.0f),m_fAirForce(0.0f),
					m_bakOption(HitOption::MULTYFRAME),m_hitType(AttackType::NONE),m_fHitTime(0.0f)
{
	//m_fGravity(-200.0f), m_fYLimit(40.0f),
	//m_fMoveSpeed(0.0f),
}

CObject::~CObject()
{
}

Rect CObject::getBoundZ() const
{
	Rect rect = Rect();

	if (m_pZPic)
	{
		Vec2 vPos = m_pZPic->convertToWorldSpace(m_pZPic->getBoundingBox().origin);

		rect = Rect(vPos.x, vPos.y, m_pZPic->getBoundingBox().size.width, m_pZPic->getBoundingBox().size.height);
		//Rect(vPos.x, vPos.y, m_pZPic->getBoundingBox().size.width, m_pZPic->getBoundingBox().size.height);
	}

	return rect;
}

CObject * CObject::cloneObj()
{
	CObject * pObject = CObject::create();

	pObject->retain();

	pObject->cloneDefaluts(this);
	
	return pObject;
}

bool CObject::init()
{
	if(!Node::init())
		return false;

	m_pYNode = Node::create();
	m_pYNode->setAnchorPoint(Vec2::ZERO);
	m_pYNode->setPosition(Vec2::ZERO);
	addChild(m_pYNode);

	m_GData.fYFloatMin = 100.0f;
	m_GData.fYLimit = 64.0f;
	m_GData.fGravity = -200.0f;
	return true;
}

void CObject::cloneDefaluts(CObject * pObj)
{
	pObj->setGravity(m_GData.fGravity);
	pObj->setDirection(m_Dir);
	pObj->initHpData(m_hpData.fMaxHp,m_hpData.fCurMp);
}

void CObject::execute(float fDelta)
{
}

void CObject::hitoptionFunc(float fDelta)
{
	if (m_bakOption == HitOption::ONEFRAME)
	{
		m_fHitTime += fDelta;

		if (m_fHitTime >= 0.25f)
		{
			m_fHitTime = 0.0f;
			m_bakOption = HitOption::MULTYFRAME;
			m_hitType = AttackType::NONE;
		}
	}
}

void CObject::gravityFunc(float fDelta)
{
	if (m_isGravityPause)
		return;

	if (m_GData.isAir)
	{
		if (m_pYNode->getPosition().y >= m_GData.fYLimit)
		{
			m_fAirForce = -200.0f;
		}

		if (m_pYNode->getPosition().y <= 0.0f)
		{
			m_GData.isAir = false;
		}
	}
	else
	{
		m_fAirForce = -200.0f;
	}

	m_fAirForce += m_GData.fGravity * fDelta * 1.5f;

	if (m_fAirForce < -400.0f)
		m_fAirForce = -400.0f;

	if (m_GData.isAir)
	{
		float fFloat = 0.0f;

		fFloat = m_fAirForce * fDelta;

		Vec2 floatPos = m_pYNode->getPosition();
		floatPos.y += fFloat;

		m_pYNode->setPosition(floatPos);

		if (m_pYNode->getPosition().y < 0.0f)
			m_pYNode->setPosition(m_pYNode->getPosition().x, 0.0f);
	}
}

void CObject::createNodeInfo(std::string szFilePath)
{
	if (m_nodeInfo.pNode)
		return;

	m_nodeInfo.pNode = CSLoader::createNode(szFilePath);
	m_pYNode->addChild(m_nodeInfo.pNode);

	m_nodeInfo.pTimeline = CSLoader::createTimeline(szFilePath);
	m_nodeInfo.pNode->runAction(m_nodeInfo.pTimeline);

	m_nodeInfo.szFilePath = szFilePath;
}

void CObject::initHpData(float fHp,float fMp)
{
	if (m_hpData.fMaxHp > 0.0f)
		return;
	if (m_hpData.fMaxMp >= 0.0f)
		return;

	m_hpData = ObjHpData(fHp,fMp);
}

bool CObject::checkHitboxBound(Rect zRect, Rect yRect)
{
	return false;
}

void CObject::hpChange(float fHpSrc, AttackType type, HitOption option)
{
	m_hpData.fCurHp -= fHpSrc;
	m_bakOption = option;

	if (m_hpData.fCurHp > m_hpData.fMaxHp)
		m_hpData.fCurHp = m_hpData.fMaxHp;
	else if (m_hpData.fCurHp < 0.0f)
		m_hpData.fCurHp = 0.0f;
}
