#include "EnemyShooter.h"
#include "PlayerMgr.h"
#include "StageMgr.h"
#include "Projectile.h"

CEnemyShooter::CEnemyShooter()
{
}

CEnemyShooter::~CEnemyShooter()
{
}

bool CEnemyShooter::init(unsigned int nIdx)
{
	if(!CEnemy::init(nIdx))
		return false;

	setState(State::IDLE);

	m_type = EnemyType::SHOOTER;

	m_isDead = false;

	return true;
}

CEnemyShooter * CEnemyShooter::create(unsigned int idx)
{
	CEnemyShooter *pRet = new(std::nothrow) CEnemyShooter();
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

void CEnemyShooter::execute(float fDelta)
{
	if (!m_pRoom)
		return;
	if (m_isDead)
		return;

	CPlayer* pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();
	CStageMap* pMap = CStageMgr::getInst()->getStageMap();;
	
	if (pMap->getCurrentRoom() != m_pRoom)
		return;
	if (!pPlayer)
		return;
	if (pPlayer->getIsDead())
		return;

	hitoptionFunc(fDelta);
	float fDistance = getPosition().getDistance(pPlayer->getPosition());

	
	switch (m_stateIdx)
	{
	case State::IDLE:
		if ( fDistance >= 200.0f || fDistance <= 150.0f)
		{
			m_fWaitTime += fDelta;

			if (m_fWaitTime >= 1.5f)
			{
				setState(State::ATTACK);
				m_fWaitTime = 0.0f;
			}
		}
		else 
		{
			setState(State::MOVE);
			m_fWaitTime = 0.0f;
		}
		break;
	case State::MOVE:
		if (fDistance >= 200.0f)
		{
			setState(State::ATTACK);
			m_fWaitTime = 0.0f;
		}
		else
		{
			if (fDistance >= 300.0f || fDistance<=150.0f)
			{
				setState(State::IDLE);
				break;
			}

			m_destPos = getPosition() - pPlayer->getPosition();
			
			if (getPosition().distance(m_destPos) <= 0.5f)
			{
				setState(State::IDLE);
				break;
			}

			m_Dir = m_destPos;
			m_Dir.normalize();

			Vec2 movePos = m_Dir * m_fMoveSpeed;

			movePos = getPosition() + movePos;

			roomInsectCheck(movePos);

			setPosition(movePos);

		}
		break;
	case State::DOWN:
		m_fWaitTime += fDelta;
		if (m_fWaitTime > ENEMY_DEFAULT_WAITTIME)
		{
			m_fWaitTime = 0.0f;
			setState(State::IDLE);
			break;
		}
		break;
	}

	if (m_Dir.x >= 0)
	{
		m_nodeInfo.pNode->setScaleX(1.0f);
	}
	else
	{
		m_nodeInfo.pNode->setScaleX(-1.0f);
	}

	CEnemy::gravityFunc(fDelta);
}

void CEnemyShooter::setState(State idx)
{
	CEnemy::setState(idx);

	CPlayer* pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();
	if (!pPlayer)
		return;

	switch (m_stateIdx)
	{
	case State::IDLE:
		m_nodeInfo.pTimeline->play("idle", true);
		break;
	case State::MOVE:
	{
		m_nodeInfo.pTimeline->play("run", true);
	}
	break;
	case State::ATTACK:
	{
		m_nodeInfo.pTimeline->play("atk_00", false);
		m_Dir = pPlayer->getPosition() - getPosition();
		m_Dir.normalize();
	}
	break;
	case State::HIT:
	{
		switch (m_hitType)
		{
		case AttackType::FLOAT:
			m_nodeInfo.pTimeline->play("hit_01", false);
			m_GData.isAir = true;
			m_fAirForce = 150.0f;
			m_pYNode->runAction(MoveBy::create(0.0f, Vec2(0.0f, 1.0f)));
			break;
		case AttackType::PUSH:
			m_nodeInfo.pTimeline->play("hit_00", false);
			runAction(MoveBy::create(0.1f, -m_Dir * 50.0f));
			break;
		default:
			m_nodeInfo.pTimeline->play("hit_00", false);
			break;
		}
	}
	break;
	case State::DEAD:
	{
		m_nodeInfo.pTimeline->play("dead", false);
	}
	break;
	}
}
