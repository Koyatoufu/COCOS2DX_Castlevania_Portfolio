#include "EnemyRunner.h"
#include "PlayerMgr.h"
#include "StageMgr.h"

CEnemyRunner::CEnemyRunner():CEnemy()
{
}

CEnemyRunner::~CEnemyRunner()
{
}

bool CEnemyRunner::init(unsigned int nIdx)
{
	if (!CEnemy::init(nIdx))
		return false;

	m_type = EnemyType::RUNNER;

	setState(State::IDLE);

	m_isDead = false;

	return true;
}

CEnemyRunner * CEnemyRunner::create(unsigned int idx)
{
	CEnemyRunner *pRet = new(std::nothrow) CEnemyRunner();
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

void CEnemyRunner::execute(float fDelta)
{
	if (!m_pRoom)
		return;
	if (m_isDead)
		return;

	CPlayer* pPlayer=CPlayerMgr::getInst()->getCurrentPlayer();
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
		m_fWaitTime += fDelta;
		if (m_fWaitTime <= ENEMY_DEFAULT_WAITTIME)
			break;
		if (fDistance <= 200.0f)
		{
			setState(State::MOVE);
			m_fWaitTime = 0.0f;
		}
		break;
	case State::MOVE:
		if (fDistance <= 60.0f || fDistance >= 500.0f)
		{
			setState(State::ATTACK);
		}
		else 
		{
			if (fDistance > 200.0f || m_destPos.distance(getPosition()) <= 10.0f)
			{
				setState(State::IDLE);
				break;
			}
			
			Vec2 dir = pPlayer->getPosition() - getPosition();
			dir.normalize();
			m_Dir = dir;

			Vec2 movePos = dir * m_fMoveSpeed;

			movePos = getPosition() + movePos;

			roomInsectCheck(movePos);

			setPosition(movePos);
			
		}
	break;
	case State::ATTACK:
	{
		Vec2 dir = m_destPos;

		Vec2 movePos = dir * m_fMoveSpeed;
		movePos = getPosition() + movePos;
		roomInsectCheck(movePos);
		setPosition(movePos);

		float fFloat = 0.0f;

		fFloat = m_fAirForce * fDelta;

		Vec2 floatPos = m_pYNode->getPosition();
		floatPos.y += fFloat;

		m_pYNode->setPosition(floatPos);

		if (m_pYNode->getPosition().y < 0.0f)
		{
			m_pYNode->setPosition(m_pYNode->getPosition().x, 0.0f);
		}
	}
		break;
	case State::HIT:
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
	case State::DEAD:
		break;
	}

	if (m_Dir.x >= 0)
	{
		m_pSkeleton->setScaleX(1.0f);
	}
	else 
	{
		m_pSkeleton->setScaleX(-1.0f);
	}

	CEnemy::gravityFunc(fDelta);
}

void CEnemyRunner::setState(State idx)
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
			
			m_Dir = pPlayer->getPosition() - getPosition();
			m_Dir.normalize();
		}
		break;
	case State::ATTACK:
		{
			m_nodeInfo.pTimeline->play("atk_00", false);
			m_destPos = pPlayer->getPosition() - getPosition();
			m_destPos.normalize();
			m_fAirForce = 100.0f;
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
				runAction(MoveBy::create(0.5f,-m_Dir*50.0f));
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
			m_isDead = true;
		}
		break;
	}
}