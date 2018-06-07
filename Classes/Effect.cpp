#include "Effect.h"
#include "DataMgr.h"
#include "ObjectPool.h"
#include "StageMgr.h"
#include "SoundMgr.h"

CEffect::CEffect():CObject(),m_fLiveTime(0.0f),m_fLimitTime(0.0f),m_data()
{
}

CEffect::~CEffect()
{
}

CEffect * CEffect::create(unsigned int idx)
{
	CEffect *pRet = new(std::nothrow) CEffect();
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

bool CEffect::init(unsigned int nIdx)
{
	if (!CObject::init())
		return false;

	m_nIdx = nIdx;

	m_data = CDataMgr::getInst()->getObjData()->getEffectData(nIdx);

	createNodeInfo(m_data.filePath);

	m_fLimitTime = m_data.fLiveTime;
	m_fLiveTime = 0.0f;

	return true;
}

void CEffect::execute(float fDelta)
{
	if (!isVisible())
		return;

	if (m_fLiveTime > m_fLimitTime)
	{
		poolThis();
		return;
	}

	m_fLiveTime += m_fLimitTime;
}

void CEffect::startEffects(Vec2 zPos, Vec2 yPos)
{
	setVisible(true);
	m_fLiveTime = 0.0f;
	m_nodeInfo.pTimeline->play("play",true);

	auto pMap = CStageMgr::getInst()->getStageMap();
	pMap->addObjectFromList(this);
	pMap->addChild(this);

	setPosition(zPos);
	m_pYNode->setPosition(yPos);

	setVisible(true);

	CSoundMgr::getInst()->playEffect(StringUtils::format("sounds/effects/se_%02d.ogg", m_data.nSeIdx));
}

void CEffect::poolThis()
{
	CStageMgr::getInst()->getStageMap()->removeObjectFromList(this);
	CObjectPool::getInst()->PoolObject(this, ObjectType::EFFECT, m_nIdx);
}
