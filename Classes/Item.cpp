#include "Item.h"
#include "DataMgr.h"
#include "PlayerMgr.h"
#include "StageMgr.h"
#include "ObjectPool.h"
#include "SoundMgr.h"

#define ITEM_BODY 99
#define SUBWEAPON_LENGTH 3
#define LIVETIME_LIMIT 5.0f

CItem::CItem():CObject(),m_data(),m_fLiveTime(0.0f)
{
}

CItem::~CItem()
{
}

bool CItem::init(unsigned int nIdx)
{
	if(!CObject::init())
		return false;

	m_data = CDataMgr::getInst()->getObjData()->getItemData(nIdx);

	createNodeInfo(m_data.filePath);
	m_pSpr = (Sprite*)m_nodeInfo.pNode->getChildByTag(ITEM_BODY);

	m_nodeInfo.pTimeline->play("idle", true);

	if (m_data.type == ItemType::SUBWEAPONCHANGE)
	{
		int nRate = rand() % 100;

		if (nRate >= 70)
			m_data.nValue = 2;
		else if (nRate >= 40)
			m_data.nValue = 1;
		else
			m_data.nValue = 0;
		
		m_nodeInfo.pTimeline->play(StringUtils::format("idle_%02d", m_data.nValue), true);
	}

	m_pZPic = Sprite::create("common/empty.png");
	m_pZPic->setAnchorPoint(Vec2::ZERO);
	addChild(m_pZPic);

	return true;
}

CItem * CItem::create(unsigned int idx)
{
	CItem *pRet = new(std::nothrow) CItem();
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

void CItem::execute(float fDelta)
{
	auto pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();
	
	if (!pPlayer)
		return;
	if (pPlayer->getIsDead())
		return;

	if (pPlayer->checkHitboxBound(getBoundZ(),getItemRect()))
	{
		useItem(pPlayer);
		return;
	}

	m_fLiveTime += fDelta;
	if (m_fLiveTime >= LIVETIME_LIMIT)
	{
		if (m_data.type == ItemType::ORB)
		{
			if (pPlayer->getStateIdx() != State::DEAD)
				CStageMgr::getInst()->stageClearExecute();
		}
		pooledThis();
	}
}

void CItem::resetItem(Vec2 pos)
{
	setPosition(pos);
	CStageMgr::getInst()->getStageMap()->addChild(this);
	CStageMgr::getInst()->getStageMap()->addObjectFromList(this);
	m_fLiveTime = 0.0f;
	setVisible(true);
}

void CItem::useItem(CPlayer *pPlayer)
{
	if (!isVisible())
		return;

	switch (m_data.type)
	{
	case ItemType::HEAL:
		pPlayer->hpChange(-m_data.nValue, AttackType::NONE, HitOption::MULTYFRAME);
		break;
	case ItemType::CHARGE:
		pPlayer->mpChange(-m_data.nValue);
		break;
	case ItemType::SUBWEAPONCHANGE:
		pPlayer->changeSubWeaponIdx(m_data.nValue);
		break;
	case ItemType::ORB:
		CStageMgr::getInst()->stageClearExecute();
		break;
	}

	CSoundMgr::getInst()->playEffect(StringUtils::format("sounds/effects/se_%02d.ogg", m_data.nSeIdx));
	pooledThis();
}

Rect CItem::getItemRect()
{
	Rect rect = m_pSpr->getBoundingBox();
	rect.origin = convertToWorldSpace(rect.origin);

	return rect;
}

void CItem::pooledThis()
{
	CStageMgr::getInst()->getStageMap()->removeObjectFromList(this);
	CObjectPool::getInst()->PoolObject(this, ObjectType::ITEM, m_nIdx);
	setVisible(false);
}
