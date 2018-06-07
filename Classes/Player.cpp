#include "Player.h"
#include "PlayerMgr.h"
#include "DataMgr.h"
#include "StageMgr.h"
#include "SoundMgr.h"

CPlayer::CPlayer():CUnit(),m_isAct(false),m_isHitable(true),m_isAtkable(true),m_nAtkNum(0),m_pWeapon(0)
{
}

CPlayer::~CPlayer()
{
}

bool CPlayer::init(unsigned int idx)
{
	if ((PLUnitIdx)idx == PLUnitIdx::NONE)
		return false;

	CDataMgr *pDataMgr = CDataMgr::getInst();
	CObjectData *pObjData = pDataMgr->getObjData();
	PlayerUnitData playerData = pObjData->getPlayerUnitData((unsigned int)idx);

	if (!CUnit::init(playerData.filePath))
		return false;

	m_nIdx = idx;

	m_pZPic->setColor(Color3B(125, 200, 255));
	m_pZPic->setOpacity(90);

	m_stateIdx = State::IDLE;

	m_nodeInfo.pTimeline->play("idle", true);

	m_nSubIdx = 0;

	m_pWeapon = CWeapon::create((unsigned int)playerData.nWeaponIdx);
	m_pWeapon->setUnitOwner(this);
	m_pSkeleton->getBoneNode("bone_attack")->addChild(m_pWeapon);
	m_pZPic->setScaleX(m_pSkeleton->getBoneNode("bone_root")->getChildByTag(HITBOX)->getScaleX());

	m_GData.isAir = false;
	m_isJumpStart = false;

	m_Dir = Vec2(1.0f, 0.0f);
	m_Dir.normalize();

	m_fMoveSpeed = playerData.fMoveSpeed;

	m_hpData = playerData.hpData;

	m_unitStatus = playerData.status;

	return true;
}

CPlayer * CPlayer::create(unsigned int idx)
{
	CPlayer *pRet = new(std::nothrow) CPlayer();
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

void CPlayer::execute(float fDelta)
{
	if (!m_isAct)
		return;
	if (m_isDead)
		return;

	hitoptionFunc(fDelta);

	float fVelocity = 0.0f;

	switch (m_stateIdx)
	{
	case State::IDLE:
		break;
	case State::MOVE:
		fVelocity =  m_fMoveSpeed; 
		if (m_Dir.getLength() <= DIR_NOR_LENGTH)
		{
			setState(State::IDLE);
			return;
		}
		break;
	case State::JUMP:
		fVelocity = m_fMoveSpeed;
		if (!m_GData.isAir)
		{
			if (!m_isJumpStart)
			{
				m_fAirForce = 200.0f;
				m_GData.isAir = true;
				m_isJumpStart = true;
				m_Dir.y = 0.0f;
				m_pYNode->setPositionY(1.0f);
				break;
			}
		}
		
		if (m_pYNode->getPosition().y <= 0.0f)
		{
			setState(State::IDLE);
			m_isJumpStart = false;
		}
		break;
	case State::DOWN:
		if (m_pYNode->getPosition().y <= 0.0f)
		{
			if (!m_nodeInfo.pTimeline->isPlaying())
			{
				m_nodeInfo.pTimeline->play("wakeup", false);
			}
		}
	}

	if (m_Dir.x < 0)
		m_nodeInfo.pNode->setScaleX(-1.0f);
	else
		m_nodeInfo.pNode->setScaleX(1.0f);

	m_pWeapon->setDirection(m_Dir);
	
	Vec2 movePos = m_Dir * fVelocity;

	movePos = getPosition() + movePos;

	roomInsectCheck(movePos);

	setPosition(movePos);
	gravityFunc(fDelta);

}

void CPlayer::checkFrameEvent(Frame* pFrame)
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

	if (strings[0].compare("voice") == 0)
	{
		str = StringUtils::format("sounds/voice/voice_%s.ogg", strings[1].c_str());
		CSoundMgr::getInst()->playEffect(str);
	}
	if (strings[0].compare("se") == 0)
	{
		CSoundMgr::getInst()->playEffect(StringUtils::format("sounds/effects/se_%s.ogg", strings[1].c_str()));
	}

	if (strings[0].compare("end") == 0)
	{
		m_isAtkable = true;
		m_nAtkNum = 0;
		m_isGravityPause = false;
		m_isHitable = true;

		if (m_GData.isAir)
			setState(State::JUMP);
		else 
			setState(State::IDLE);
		return;
	}

	if (strings[0].compare("dead") == 0)
	{
		m_isDead = true;
		CPlayerMgr::getInst()->deadAccept(this);
		setVisible(false);
		return;
	}

	switch (m_stateIdx)
	{
	break;
	case State::ATTACK:
		if (strings[0].compare("cancel") == 0)
		{
			m_isAtkable = true;
			break;
		}
	case State::SUBATTACK:
		if (strings[0].compare("shoot") == 0)
		{
			shootProjectile(ProjectileTarget::OTHER);
			m_isAtkable = false;
		}
		else if (strings[0].compare("rapid") == 0)
		{
			m_isAtkable = true;
		}
		break;
	case State::SKILL:
		if (strings[0].compare("shoot") == 0)
			shootProjectile(ProjectileTarget::OTHER);
		if (strings[0].compare("cost") == 0)
		{
			auto data = CDataMgr::getInst()->getObjData()->getProjectileData(m_nSubIdx);
			mpChange(data.fCost * 5.0f);
		}
		if (strings[0].compare("unhitable") == 0)
			m_isHitable = false;
		if (strings[0].compare("hitable") == 0)
			m_isHitable = true;

		break;
	}
}


void CPlayer::setState(State idx)
{
	CUnit::setState(idx);
	
	Vec2 dirTmp = m_Dir;

	switch (idx)
	{
	case State::IDLE:
		m_nodeInfo.pTimeline->play("idle", true);
		m_isAtkable = true;
		m_nAtkNum = 0;
		break;
	case State::MOVE:
		m_nodeInfo.pTimeline->play("run", true);
		m_isAtkable = true;
		m_nAtkNum = 0;
		break;
	case State::JUMP:
		m_nodeInfo.pTimeline->play("jump", false);
		m_isAtkable = true;
		m_nAtkNum = 0;
		break;
	case State::EVADE:
		if (m_Dir.length() <= DIR_NOR_LENGTH)
		{
			m_nodeInfo.pTimeline->play("evade_back", false);
			dirTmp.y = 0.0f;
			dirTmp *= -80.0f;
			runAction(MoveBy::create(0.5f, dirTmp));
			m_pYNode->runAction(JumpBy::create(0.4f, Vec2::ZERO, 30.0f, 1));
		}
		else
		{
			m_nodeInfo.pTimeline->play("evade_front", false);
			dirTmp *= 100.0f;
			runAction(MoveBy::create(0.4f, dirTmp));
		}
		break;
	case State::ATTACK:
		if (m_isAtkable)
		{
			if (m_GData.isAir)
			{
				m_nodeInfo.pTimeline->play(StringUtils::format("jump_atk_%02d", m_nAtkNum), false);
				m_pWeapon->playAnimation(StringUtils::format("jump_atk_%02d", m_nAtkNum), false);

				if (m_nAtkNum == 1)
					runAction(MoveBy::create(0.4f, Vec2(m_Dir.x *50.0f, 0.0f)));
			}
			else
			{
				m_nodeInfo.pTimeline->play(StringUtils::format("atk_%02d", m_nAtkNum), false);
				m_pWeapon->playAnimation(StringUtils::format("atk_%02d", m_nAtkNum), false);

				if (m_nAtkNum == 2)
					runAction(MoveBy::create(0.51f, Vec2(m_Dir.x * 100.0f, 0.0f)));
				else if (m_nAtkNum == 3)
				{
					runAction(MoveBy::create(0.45f, Vec2(m_Dir.x *50.0f, 0.0f)));
					m_pYNode->runAction(JumpBy::create(0.45f, Vec2::ZERO, 20.0f, 1));
				}
			}
			m_nAtkNum++;
			m_isAtkable = false;
		}
		break;
	case State::SUBATTACK:
		if (m_isAtkable)
		{
			auto data = CDataMgr::getInst()->getObjData()->getProjectileData(m_nSubIdx);
			if (m_hpData.fCurMp - data.fCost < 0.0f)
			{
				setState(State::IDLE);
				break;
			}

			if (m_GData.isAir)
			{
				m_nodeInfo.pTimeline->play("jump_sub", false);
				m_pWeapon->playAnimation("atk_sub", false);
			}
			else
			{
				m_nodeInfo.pTimeline->play("atk_sub", false);
				m_pWeapon->playAnimation("atk_sub", false);
			}

			m_isAtkable = false;
		}
		break;
	case State::SKILL:
		{
			auto data = CDataMgr::getInst()->getObjData()->getProjectileData(m_nSubIdx);
			if (m_hpData.fCurMp - data.fCost * 5.0f < 0.0f)
			{
				setState(State::IDLE);
				break;
			}

			m_nodeInfo.pTimeline->play(StringUtils::format("sp_%02d",m_nSubIdx), false);
			m_pWeapon->playAnimation(StringUtils::format("sp_%02d", m_nSubIdx), false);

			m_isAtkable = false;
		}
		break;
	case State::HIT:

		m_pWeapon->playAnimation("idle", false);
		m_isAtkable = false;
		m_nAtkNum = 0;
		stopAllActions();

		switch (m_hitType)
		{
		case AttackType::FLOAT:
			m_pYNode->runAction(MoveBy::create(0.0f,Vec2(0.0f,1.0f)));
			m_GData.isAir = true;
			m_fAirForce = 150.0f;
			setState(State::DOWN);
			break;
		case AttackType::NONE:
			m_nodeInfo.pTimeline->play("hit_00", false);
			break;
		case AttackType::PUSH:
			m_nodeInfo.pTimeline->play("hit_00", false);
			Vec2 movePos = m_Dir * -1.0f * 10.0f;
			movePos += getPosition();
			roomInsectCheck(movePos);
			runAction(MoveTo::create(0.2f, movePos));
			break;
		}
		break;
	case State::DOWN:
		m_nodeInfo.pTimeline->play("down", false);
		break;
	case State::DEAD:
		m_nodeInfo.pTimeline->play("dead", false);
		m_isHitable = false;
		break;
	}
}

void CPlayer::changeSubWeaponIdx(unsigned int nIdx)
{
	PlayerUnitData data = CDataMgr::getInst()->getObjData()->getPlayerUnitData(m_nIdx);
	if (nIdx >= data.subWeaponIdx.size())
		nIdx = data.subWeaponIdx.size() - 1;

	m_nSubIdx = nIdx;
	CStageMgr::getInst()->changeSubWeaponImg(nIdx);
}

void CPlayer::roomInsectCheck(Vec2 & pos)
{
	auto pMap = CStageMgr::getInst()->getStageMap();;
	auto pRoom = pMap->getCurrentRoom();
	
	if (!pRoom)
		return;

	int nXpos = (pos.x - pRoom->getPosition().x) / FLOOR_TILE_SIZE;
	int nYpos = (pos.y - pRoom->getPosition().y) / FLOOR_TILE_SIZE;

	if (pRoom->getTileReg(nXpos, nYpos) == TileReg::BLOCK)
	{
		Rect tileRect = Rect(nXpos * FLOOR_TILE_SIZE, nYpos * FLOOR_TILE_SIZE, FLOOR_TILE_SIZE, FLOOR_TILE_SIZE);

		pos = getPosition();
	}

	Rect rect = pRoom->getFloorBoundRect();

	if (!rect.containsPoint(pos))
	{
		if (pos.x > pRoom->getPosition().x + rect.getMaxX())
			pos.x = pRoom->getPosition().x + rect.getMaxX();
		else if (pos.x < pRoom->getPosition().x + rect.getMinX())
			pos.x = pRoom->getPosition().x + rect.getMinX();

		if (pos.y > pRoom->getPosition().y + rect.getMaxY())
			pos.y = pRoom->getPosition().y + rect.getMaxY();
		else if (pos.y < pRoom->getPosition().y + rect.getMinY())
			pos.y = pRoom->getPosition().y + rect.getMinY();
	}
}

void CPlayer::hpChange(float fHpSrc, AttackType type, HitOption option)
{
	if (m_stateIdx == State::DEAD || !m_isHitable || m_stateIdx == State::EVADE )
		return;

	if (m_bakOption == HitOption::ONEFRAME && m_bakOption == option)
		return;

	CObject::hpChange(fHpSrc, type , option);

	if (fHpSrc >= 0.0f)
	{
		m_hitType = type;
		setState(State::HIT);
	}

	if (m_hpData.fCurHp <= 0.0f)
		setState(State::DEAD);
}

/*if (tileRect.containsPoint(pos))
{
if (pos.x > tileRect.getMinX() && pos.x < tileRect.getMidX())
{
pos.x = tileRect.getMinX();
}
else if (pos.x < tileRect.getMaxX() && pos.x > tileRect.getMidX())
{
pos.x = tileRect.getMaxX();
}

if (pos.y > tileRect.getMinY() && pos.y < tileRect.getMidY())
{
pos.y = tileRect.getMinY();
}
else if (pos.y < tileRect.getMaxY() && pos.y > tileRect.getMidY())
{
pos.y = tileRect.getMaxY();
}
}*/