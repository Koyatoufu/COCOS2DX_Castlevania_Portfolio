#include "Unit.h"
#include "ObjectPool.h"
#include "Projectile.h"

CUnit::CUnit() : CObject(),m_unitStatus(),m_stateIdx(),m_destPos(),m_isDead(false),m_pHpTimer(0),m_pHpBack(0),m_nSubIdx(100)
{
}

CUnit::~CUnit()
{
}

bool CUnit::init(std::string szFilePath)
{
	if (!CObject::init())
		return false;

	m_pZPic = Sprite::create("common/zcircle.png");
	m_pZPic->setAnchorPoint(Vec2(0.5f, 0.5f));
	m_pZPic->setPosition(Vec2::ZERO);
	addChild(m_pZPic, -2);

	m_stateIdx = State::NONE;

	createNodeInfo(szFilePath);

	return true;
}

CUnit * CUnit::create(std::string szFilePath)
{
	CUnit *pRet = new(std::nothrow) CUnit();
	if (pRet && pRet->init(szFilePath))
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

CUnit * CUnit::cloneObj()
{
	CUnit* pUnit = CUnit::create(m_nodeInfo.szFilePath);

	pUnit->cloneDefaluts(this);

	return pUnit;
}

void CUnit::checkFrameEvent(Frame* pFrame)
{

}

void CUnit::roomInsectCheck(Vec2 & pos)
{

}

void CUnit::cloneDefaluts(CUnit * pObj)
{
	CObject::cloneDefaluts(pObj);
	
	pObj->m_unitStatus = m_unitStatus;
}

void CUnit::setState(State idx)
{
	m_stateIdx = idx;
}

State CUnit::getStateIdx() const
{
	return m_stateIdx;
}

void CUnit::setSkeltonPath(std::string szFilePath)
{
	if (!m_nodeInfo.szFilePath.empty())
		return;

	m_nodeInfo.szFilePath = szFilePath;
}

void CUnit::createNodeInfo(std::string szFilePath)
{
	CObject::createNodeInfo(szFilePath);

	m_pSkeleton = (SkeletonNode*)m_nodeInfo.pNode;

	setSkeltonPath(szFilePath);

	m_nodeInfo.pTimeline->setFrameEventCallFunc(CC_CALLBACK_1(CUnit::checkFrameEvent, this));
}

bool CUnit::checkHitboxBound(Rect zRect, Rect yRect)
{
	Rect zBound = getBoundZ();

	zBound.origin.x = zRect.origin.x;

	Rect hitbox = getHitboxBounding();
	
	if (zBound.intersectsRect(zRect))
	{
		if (hitbox.intersectsRect(yRect))
		{
			return true;
		}
	}

	return false;
}

void CUnit::hpChange(float fHpSrc, AttackType type, HitOption option)
{
	if (m_stateIdx == State::DEAD || m_stateIdx == State::EVADE)
		return;

	if (m_bakOption == HitOption::ONEFRAME && m_bakOption == option)
		return;

	CObject::hpChange(fHpSrc, type, option);

	if (fHpSrc >= 0.0f)
	{
		setState(State::HIT);
		m_hitType = type;
	}

	if (m_hpData.fCurHp <= 0.0f)
		setState(State::DEAD);
}

void CUnit::mpChange(float fCost)
{
	if (m_hpData.fCurMp - fCost <= 0.0f)
	{
		m_hpData.fCurMp = 0.0f;
		return;
	}
	if (m_hpData.fCurMp - fCost >= m_hpData.fMaxMp)
	{
		m_hpData.fCurMp = m_hpData.fMaxMp;
		return;
	}

	m_hpData.fCurMp -= fCost;
}

void CUnit::shootProjectile(ProjectileTarget target)
{
	CProjectile *pSub = (CProjectile*)CObjectPool::getInst()->getObject(ObjectType::PROJECTILE, m_nSubIdx);
	Vec2 dir = Vec2(m_Dir.x, 0.0f);
	Vec2 Zpos = Vec2(getPosition().x + dir.x * FLOOR_TILE_SIZE, getPosition().y);
	Vec2 Ypos = Vec2(0.0f, m_pSkeleton->getBoneNode("bone_attack")->getPosition().y * m_pSkeleton->getBoneNode("bone_root")->getScaleY());

	pSub->reset(this, dir, Zpos, Ypos, target);
}

Rect CUnit::getHitboxBounding()
{
	Node* pRoot = m_pSkeleton->getBoneNode("bone_root");
	Node* pNode = m_pSkeleton->getBoneNode("bone_root")->getChildByTag(HITBOX);

	Rect rect = pNode->getBoundingBox();
	Vec2 newpos = -rect.origin;
	if (m_Dir.x > 0)
		newpos = rect.origin;
	rect.origin = pRoot->convertToWorldSpace(newpos);

	rect.size.width *= pRoot->getScaleX();
	rect.size.height *= pRoot->getScaleY();

	return rect;
}

