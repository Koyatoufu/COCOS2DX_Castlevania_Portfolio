#include "Enemy.h"
#include "PlayerMgr.h"
#include "DataMgr.h"
#include "ui\UILoadingBar.h"
#include "ObjectPool.h"
#include "Projectile.h"
#include "Effect.h"
#include "SoundMgr.h"

CEnemy::CEnemy():CUnit(),m_pRoom(0),m_type(),m_nEnemyIdx(0),m_fWaitTime(0.0f),m_pAttackBox(0),m_pHpbar(0)
{
}

CEnemy::~CEnemy()
{
}


bool CEnemy::init(unsigned int nIdx)
{
	auto data = CDataMgr::getInst()->getObjData()->getEnemyUnitData(nIdx);
	m_nIdx = nIdx;

	if (!CUnit::init(data.filePath))
		return false;

	m_pZPic->setColor(Color3B::RED);
	m_pZPic->setOpacity(75);
	float fXsize = m_pSkeleton->getBoneNode("bone_root")->getChildByTag(123)->getScaleX() * m_pSkeleton->getBoneNode("bone_root")->getScaleX();
	m_pZPic->setScaleX(fXsize);

	m_pAttackBox = m_pSkeleton->getBoneNode("bone_attack")->getChildByTag(ATTACKBOX);

	m_pHpbar = ui::LoadingBar::create("common/hp_front.png", 100.0f);
	m_pHpbar->setPosition(Vec2(0.0f, m_pSkeleton->getBoneNode("bone_root")->getChildByTag(HITBOX)->getBoundingBox().size.height*2.0f + 15.0f));
	m_pHpbar->setColor(Color3B::RED);
	m_pHpbar->setPercent(100);
	m_pYNode->addChild(m_pHpbar);

	m_fMoveSpeed = data.fMoveSpeed;
	m_hpData = data.hpData;
	m_unitStatus = data.status;
	m_nSubIdx = data.nProjtileIdx;

	m_Dir.x = -1.0f;

	return true;
}

void CEnemy::setState(State idx)
{
	CUnit::setState(idx);
}

void CEnemy::hpChange(float fHpSrc, AttackType type,HitOption option)
{
	if (m_stateIdx == State::DEAD || m_stateIdx == State::EVADE)
		return;

	if (m_bakOption == option && m_bakOption == HitOption::ONEFRAME)
		return;

	CObject::hpChange(fHpSrc, type,option);

	if (fHpSrc >= 0.0f)
	{
		m_hitType = type;
		setState(State::HIT);
		m_pHpbar->setPercent(m_hpData.fCurHp / m_hpData.fMaxHp*100.0f);
	}

	if (m_hpData.fCurHp <= 0.0f)
	{
		setState(State::DEAD);
		m_pHpbar->setVisible(false);
	}
}

void CEnemy::roomInsectCheck(Vec2 & pos)
{
	if (!m_pRoom)
		return;

	Rect rect = m_pRoom->getFloorBoundRect();

	if (!rect.containsPoint(pos))
	{
		if (pos.x > m_pRoom->getPosition().x + rect.getMaxX())
		{
			pos.x = m_pRoom->getPosition().x + rect.getMaxX();
		}
		else if (pos.x < m_pRoom->getPosition().x + rect.getMinX())
		{
			pos.x = m_pRoom->getPosition().x + rect.getMinX();
		}
		if (pos.y > m_pRoom->getPosition().y + rect.getMaxY())
		{
			pos.y = m_pRoom->getPosition().y + rect.getMaxY();
		}
		else if (pos.y < m_pRoom->getPosition().y + rect.getMinY())
		{
			pos.y = m_pRoom->getPosition().y + rect.getMinY();
		}
	}
}

void CEnemy::initHereRoom(CRoom * pRoom)
{
	if (m_pRoom)
		return;

	m_pRoom = pRoom;
}

Rect CEnemy::getAttackBoxBounding()
{
	Rect rect = m_pAttackBox->getBoundingBox();

	Vec2 vPos = rect.origin;

	if (m_Dir.x < 0)
		vPos.x += rect.size.width;

	rect.origin = m_pAttackBox->convertToWorldSpace(vPos);

	return rect;
}

void CEnemy::checkFrameEvent(Frame * pFrame)
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
		m_hitType = AttackType::NONE;
		return;
	}
	if (strings[0].compare("dead") == 0)
	{
		pooledThis();
		return;
	}
	if (strings[0].compare("down") == 0)
	{
		setState(State::DOWN);
		m_fWaitTime = 0.0f;
		m_hitType = AttackType::NONE;
	}
	if (strings[0].compare("shoot") == 0)
	{
		if (m_nSubIdx != 100)
			shootProjectile(ProjectileTarget::PLAYER);
	}
	if (!strings[0].compare("attack"))
	{
		bool isBounding = CPlayerMgr::getInst()->getCurrentPlayer()->checkHitboxBound(getBoundZ(), getAttackBoxBounding());

		AttackData temp = CDataMgr::getInst()->getAttackData()->getAttackData(atoi(strings[1].c_str()), atoi(strings[2].c_str()), atoi(strings[3].c_str()));

		if (isBounding)
		{
			auto pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();
			
			if (pPlayer->getStateIdx() == State::EVADE || pPlayer->getStateIdx() == State::DEAD)
				return;

			pPlayer->hpChange(temp.nAtk, temp.type,temp.option);
			auto pEffect = (CEffect*)CObjectPool::getInst()->getObject(ObjectType::EFFECT, temp.nEffIdx);

			Vec2 zPos = CPlayerMgr::getInst()->getCurrentPlayer()->getPosition();

			Vec2 yPos = Vec2(getAttackBoxBounding().getMaxX(), getAttackBoxBounding().getMaxY());
			yPos = convertToNodeSpace(yPos);
			yPos.x = 0.0f;

			pEffect->startEffects(zPos, yPos);
		}
	}
}

void CEnemy::pooledThis()
{
	m_pRoom->minEnemyCount();
	m_pRoom->removeObjectFromList(this);
	CObjectPool::getInst()->PoolObject(this, ObjectType::UNIT, m_nIdx);
	setVisible(false);
}
