#include "Boss.h"
#include "PlayerMgr.h"
#include "StageMgr.h"
#include "DataMgr.h"
#include "Item.h"
#include "Effect.h"
#include "ObjectPool.h"
#include "SoundMgr.h"

CBoss::CBoss():CEnemy()
{
}

CBoss::~CBoss()
{
}

bool CBoss::init(unsigned int nIdx)
{
	if (!CEnemy::init(nIdx))
		return false;

	m_isDead = false;

	setState(State::IDLE);

	return true;
}

CBoss * CBoss::create(unsigned int idx)
{
	CBoss *pRet = new(std::nothrow) CBoss();
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

void CBoss::execute(float fDelta)
{
	if (!m_pRoom)
		return;
	if (m_isDead)
		return;

	CPlayer* pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();
	CStageMap* pMap = CStageMgr::getInst()->getStageMap();
	if (pMap->getCurrentRoom() != m_pRoom)
		return;
	if (!pPlayer)
		return;
	if (pPlayer->getIsDead())
		return;

	hitoptionFunc(fDelta);
	
	Vec2 dir = pPlayer->getPosition() - getPosition();
	dir.normalize();
	m_Dir = dir;

	switch (m_stateIdx)
	{
	case State::IDLE:
		m_fWaitTime += fDelta;
		if (m_fWaitTime >= 1.0f)
		{
			setState(State::MOVE);
		}
		break;
	case State::MOVE:
		srand(time(nullptr));
		
		if (pPlayer->getPosition().distance(getPosition()) <= 100.0f)
		{
			setState(State::ATTACK);
		}
		else 
		{
			Vec2 movePos = dir * m_fMoveSpeed;

			movePos = getPosition() + movePos;

			roomInsectCheck(movePos);

			setPosition(movePos);
		}
		break;
	case State::ATTACK:
		return;
	case State::DOWN:
		if (m_pYNode->getPositionY()<= 0.0f)
		{
			m_fWaitTime += fDelta;
			if (m_fWaitTime > ENEMY_DEFAULT_WAITTIME)
			{
				setState(State::IDLE);
				break;
			}
		}
		break;
	case State::DEAD:
		break;
	}

	if (m_Dir.x >= 0)
		m_nodeInfo.pNode->setScaleX(1.0f);
	else
		m_nodeInfo.pNode->setScaleX(-1.0f);

	CEnemy::gravityFunc(fDelta);
}

void CBoss::setState(State idx)
{
	CEnemy::setState(idx);
	//m_isHit = false;

	switch (idx)
	{
	case State::IDLE:
		m_nodeInfo.pTimeline->play("idle", true);
		break;
	case State::MOVE:
		m_nodeInfo.pTimeline->play("run", true);
		break;
	case State::ATTACK:
	{
		srand(time(nullptr));

		if (m_hpData.fCurHp <= m_hpData.fMaxHp * 0.5f)
		{
			int nRate = rand() % 100;
			if (nRate >= 40)
			{
				m_nodeInfo.pTimeline->play("sp_00", false);
				break;
			}
		}
		int nRand = rand() % 3;
		m_nodeInfo.pTimeline->play(StringUtils::format("atk_%02d", nRand), false);
	}
		break;
	case State::HIT:
	{
		switch (m_hitType)
		{
		case AttackType::FLOAT:
			m_nodeInfo.pTimeline->play("hit_01", false);
			m_GData.isAir = true;
			m_fAirForce = 100.0f;
			m_pYNode->setPosition(0.0f, 1.0f);
			break;
		case AttackType::PUSH:
			m_nodeInfo.pTimeline->play("hit_00", false);
			runAction(MoveBy::create(0.5f, -m_Dir * 50.0f));
			break;
		}
	}
		break;
	case State::DEAD:
		m_nodeInfo.pTimeline->play("dead", false);
		m_isDead = true;
		break;
	}
}

void CBoss::hpChange(float fHpSrc, AttackType type,HitOption option)
{
	if (m_stateIdx == State::DEAD || m_stateIdx == State::EVADE)
		return;

	if (m_bakOption == HitOption::ONEFRAME && m_bakOption == option)
		return;

	CObject::hpChange(fHpSrc, type,option);

	if (fHpSrc >= 0.0f)
	{
		m_hitType = type;
		if(type != AttackType::NONE)
			setState(State::HIT);

		m_pHpbar->setPercent(m_hpData.fCurHp / m_hpData.fMaxHp*100.0f);
	}

	if (m_hpData.fCurHp <= 0.0f)
		setState(State::DEAD);
}

Rect CBoss::getAttackBoxBounding()
{
	Rect rect = m_pAttackBox->getBoundingBox();

	Vec2 vPos = -rect.origin;
	vPos.y = rect.origin.y;
	vPos.x -= rect.size.width;

	if (m_Dir.x < 0)
	{
		vPos = rect.origin;
		vPos.x += rect.size.width;
	}

	rect.origin = m_pAttackBox->getParent()->convertToWorldSpace(vPos);
	rect.size.width *= m_pAttackBox->getParent()->getParent()->getScaleX();
	rect.size.height *= m_pAttackBox->getParent()->getParent()->getScaleY();

	return rect;
}

void CBoss::checkFrameEvent(Frame * pFrame)
{
	EventFrame* pEvent = dynamic_cast<EventFrame*>(pFrame);
	if (!pEvent)
		return;

	std::string szEvt = pEvent->getEvent();
	if (szEvt.empty())
		return;

	std::vector<std::string> strings;
	std::istringstream szStream(szEvt.c_str());
	std::string str;
	while (std::getline(szStream, str, ',')) {
		strings.push_back(str);
	}

	if (strings[0].compare("se") == 0)
	{
		CSoundMgr::getInst()->playEffect(StringUtils::format("sounds/effects/se_%s.ogg", strings[1].c_str()));
	}

	if (strings[0].compare("end") == 0)
	{
		setState(State::IDLE);
		m_fWaitTime = 0.0f;
		m_bakOption = HitOption::MULTYFRAME;
		return;
	}
	if (strings[0].compare("dead") == 0)
	{
		m_pRoom->minEnemyCount();
		
		auto pOrb = (CItem*)CObjectPool::getInst()->getObject(ObjectType::ITEM,(unsigned int)ItemType::ORB);
		pOrb->resetItem(getPosition());

		pooledThis();
		return;
	}
	if (strings[0].compare("down") == 0)
	{
		setState(State::IDLE);
		m_fWaitTime = 0.0f;
		return;
	}
	if (!strings[0].compare("attack"))
	{
		bool isBounding = CPlayerMgr::getInst()->getCurrentPlayer()->checkHitboxBound(getBoundZ(), getAttackBoxBounding());

		if (m_pAttackBox->getParent()->getPosition().y == 0)
		{
			isBounding = CPlayerMgr::getInst()->getCurrentPlayer()->checkHitboxBound(getAttackBoxBounding(),getAttackBoxBounding());
		}

		AttackData temp = CDataMgr::getInst()->getAttackData()->getAttackData(atoi(strings[1].c_str()), atoi(strings[2].c_str()), atoi(strings[3].c_str()));

		if (isBounding)
		{
			CPlayerMgr::getInst()->getCurrentPlayer()->hpChange(temp.nAtk, temp.type,temp.option);

			auto pEffect = (CEffect*)CObjectPool::getInst()->getObject(ObjectType::EFFECT, temp.nEffIdx);

			Vec2 zPos = CPlayerMgr::getInst()->getCurrentPlayer()->getPosition();

			Vec2 yPos = Vec2(getAttackBoxBounding().getMinX(), getAttackBoxBounding().getMinY());
			yPos = convertToNodeSpace(yPos);
			yPos.x = 0.0f;

			pEffect->startEffects(zPos, yPos);
		}
	}
}

