#include "Projectile.h"
#include "StageMgr.h"
#include "DataMgr.h"
#include "PlayerMgr.h"
#include "ObjectPool.h"
#include "Effect.h"
#include "SoundMgr.h"

#define PROJECTTILE_BODY 369

CProjectile::CProjectile():CObject(),m_isMovable(true),m_pOwner(0),m_startPos(),m_pSpr(0),m_fMoved(0.0f)
{
}

CProjectile::~CProjectile()
{
}

bool CProjectile::init(unsigned int nIdx)
{
	if(!CObject::init())
		return false;

	m_nIdx = nIdx;
	m_data = CDataMgr::getInst()->getObjData()->getProjectileData(nIdx);

	createNodeInfo(m_data.filePath);
	m_nodeInfo.pTimeline->play("idle", true);

	if (m_data.atkData.option == HitOption::MULTYFRAME)
	{
		m_nodeInfo.pTimeline->setFrameEventCallFunc(CC_CALLBACK_1(CProjectile::checkFrameEvent,this));
	}

	m_pSpr = (Sprite*)m_nodeInfo.pNode->getChildByTag(PROJECTTILE_BODY);

	m_pZPic = Sprite::create("common/empty.png");
	m_pZPic->setAnchorPoint(Vec2(0.5f,0.5f));
	addChild(m_pZPic);

	//tempalary z height modify
	m_pZPic->setContentSize(Size(m_pSpr->getContentSize().width,FLOOR_TILE_SIZE));

	//schedule(schedule_selector(CProjectile::execute));

	return true;
}

CProjectile * CProjectile::create(unsigned int idx)
{
	CProjectile *pRet = new(std::nothrow) CProjectile();
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

void CProjectile::reset(CUnit* pOwner,Vec2 dir,Vec2 posZ, Vec2 posY, ProjectileTarget target)
{
	setVisible(true);

	m_pOwner = pOwner;

	if(m_data.fCost>0.0f)
		m_pOwner->mpChange(m_data.fCost);

	m_Dir = dir;
	m_Dir.normalize();

	m_startPos = posZ;
	setPosition(posZ);

	m_destPos = m_startPos + m_Dir * m_data.fLength;

	m_GData.fYLimit += posY.y * 1.5f;
	m_pYNode->setPosition(posY);
	m_isMovable = true;

	m_fAirForce = m_data.fAirForce;
	m_GData.isAir = true;

	m_target = target;

	m_fMoved = 0.0f;

	auto pMap = CStageMgr::getInst()->getStageMap();
	pMap->addObjectFromList(this);
	pMap->addChild(this);

	setVisible(true);
	CSoundMgr::getInst()->playEffect(StringUtils::format("sounds/effects/se_%02d.ogg",m_data.nSEIdx));
}

void CProjectile::execute(float fDelta)
{
	if (!m_isMovable)
		return;

	if (m_Dir.x < 0)
	{
		m_pSpr->setFlippedX(true);
	}
	else
	{
		m_pSpr->setFlippedX(false);
	}

	switch (m_data.type)
	{
	case ProjectileType::NORMAL:
	{
		if (checkAttackBounding())
		{
			pooledThis();
			return;
		}
		if (m_startPos.distance(getPosition()) > m_data.fLength)
		{
			pooledThis();
			return;
		}

		movePosition();
	}
	break;
	case ProjectileType::AIRUSE:
	{
		if (checkAttackBounding())
		{
			pooledThis();
			return;
		}

		if (m_pYNode->getPositionY()<=0.0f)
		{
			//|| m_startPos.distance(getPosition()) > m_data.fLength
			pooledThis();
			return;
		}

		movePosition();
		gravityFunc(fDelta);
		//moveAirPosition(fDelta);
	}
	break;
	case ProjectileType::RETURN:

		if (m_startPos.distance(getPosition()) > m_data.fLength)
		{
			if (m_destPos.distance(getPosition()) <= 10.0f)
			{
				m_Dir *= -1;
			}
			else if (m_destPos.distance(getPosition()) >= m_data.fLength)
			{
				pooledThis();
				return;
			}
		}

		if (m_fMoved >= m_data.fLength)
		{
			if (m_pOwner->checkHitboxBound(getBoundZ(), getAttackBox()))
			{
				pooledThis();
				return;
			}
		}

		movePosition();
		break;
	default:
		break;
	}

}

void CProjectile::movePosition()
{
	Vec2 newPos = m_Dir * m_data.fMoveSpeed;
	m_fMoved += newPos.length();
	newPos = getPosition() + newPos;

	setPosition(newPos);
}

bool CProjectile::checkAttackBounding()
{
	if (m_target == ProjectileTarget::OTHER)
	{
		if (checkAttkackOtherObject())
			return true;
	}
	else
	{
		if (checkAttackPlayer())
			return true;
	}

	return false;
}

bool CProjectile::checkAttkackOtherObject()
{
	CStageMap* pStageMap = CStageMgr::getInst()->getStageMap();

	CRoom* pRoom = pStageMap->getCurrentRoom();
	if (pRoom)
	{
		for (unsigned int i = 0; i < pRoom->getObjectsList().size(); i++)
		{
			bool isBounding = pRoom->getObjectsList()[i]->checkHitboxBound(getBoundZ(), getAttackBox());

			if (isBounding)
			{
				if (!pRoom->getObjectsList()[i]->isVisible())
					continue;

				if (m_pOwner->getStateIdx() == State::DEAD)
					return false;

				auto pEffect = (CEffect*)CObjectPool::getInst()->getObject(ObjectType::EFFECT, m_data.atkData.nEffIdx);
				pEffect->startEffects(getPosition(),m_pYNode->getPosition());

				pRoom->getObjectsList()[i]->hpChange(m_data.atkData.nAtk, m_data.atkData.type,m_data.atkData.option);
				return true;
			}
		}
	}

	return false;
}

bool CProjectile::checkAttackPlayer()
{
	CStageMap* pStageMap = CStageMgr::getInst()->getStageMap();
	CRoom* pRoom = pStageMap->getCurrentRoom();

	CPlayer* pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();
	if (pPlayer->getIsDead())
	{
		return true;
	}

	bool isBounding = pPlayer->checkHitboxBound(getBoundZ(), getAttackBox());

	if (isBounding)
	{
		if (m_pOwner->getStateIdx() == State::DEAD)
			return false;

		auto pEffect = (CEffect*)CObjectPool::getInst()->getObject(ObjectType::EFFECT, m_data.atkData.nEffIdx);
		pEffect->startEffects(getPosition(), m_pYNode->getPosition());

		pPlayer->hpChange(m_data.atkData.nAtk, AttackType::NONE, m_data.atkData.option);
		return true;
	}

	return false;
}

Rect CProjectile::getAttackBox()
{
	Node* pAtkNode = m_nodeInfo.pNode->getChildByTag(ATTACKBOX);

	Rect rect = pAtkNode->getBoundingBox();

	//it will be problem here
	rect.origin = pAtkNode->convertToWorldSpace(rect.origin);

	return rect;

}

void CProjectile::checkFrameEvent(Frame * pFrame)
{
	EventFrame* pEvent = dynamic_cast<EventFrame*>(pFrame);
	if (!pEvent)
		return;

	std::string szEvt = pEvent->getEvent();
	if (szEvt.empty())
		return;

	if (szEvt.compare("attack") == 0)
	{
		checkAttackBounding();
	}
}

void CProjectile::pooledThis()
{
	m_isMovable = false;

	CStageMgr::getInst()->getStageMap()->removeObjectFromList(this);
	CObjectPool::getInst()->PoolObject(this, ObjectType::PROJECTILE, m_nIdx);
	//removeFromParentAndCleanup(true);
}
