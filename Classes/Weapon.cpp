#include "Weapon.h"
#include "StageMgr.h"
#include "ObjectPool.h"
#include "Effect.h"
#include "DataMgr.h"
#include "SoundMgr.h"

CWeapon::CWeapon() : CObject(), m_pOwner(0), m_pStageMap(0), m_wData()
{
}

CWeapon::~CWeapon()
{
}

bool CWeapon::init(unsigned int nIdx)
{
	if(!CObject::init())
		return false;
	
	CDataMgr *pDataMgr = CDataMgr::getInst();
	CObjectData *pObjData = pDataMgr->getObjData();
	m_wData = pObjData->getWeaponData(nIdx);

	createNodeInfo(m_wData.filePath);
	m_nodeInfo.pTimeline->play("idle", true);
	m_nodeInfo.pTimeline->setFrameEventCallFunc(CC_CALLBACK_1(CWeapon::checkFrameEvent,this));

	m_pAttackBox = (Sprite*)m_nodeInfo.pNode->getChildByTag(ATTACKBOX);

	return true;
}

CWeapon * CWeapon::create(unsigned int idx)
{
	CWeapon *pRet = new(std::nothrow) CWeapon();
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

void CWeapon::playAnimation(std::string szAnimName,bool isLoop)
{
	m_nodeInfo.pTimeline->play(szAnimName,isLoop);
}

void CWeapon::checkFrameEvent(Frame * pFrame)
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

	if (!strings[0].compare("attack"))
	{
		if (!m_pStageMap)
			m_pStageMap = CStageMgr::getInst()->getStageMap();

		CRoom* pRoom = m_pStageMap->getCurrentRoom();
		if (pRoom)
		{
			for (unsigned int i = 0; i < pRoom->getObjectsList().size(); i++)
			{
				Rect zBound = m_pOwner->getBoundZ();
				zBound.size.height += m_pAttackBox->getBoundingBox().size.height * 0.125f;
				Rect yBound = getAttackBoxBounding();

				AttackData temp = CDataMgr::getInst()->getAttackData()->getAttackData(atoi(strings[1].c_str()), atoi(strings[2].c_str()), atoi(strings[3].c_str()));

				bool isBounding = pRoom->getObjectsList()[i]->checkHitboxBound(zBound, yBound);

				if (isBounding)
				{
					CObject* pObject = pRoom->getObjectsList()[i];
					
					if (dynamic_cast<CUnit*>(pObject))
						if (dynamic_cast<CUnit*>(pObject)->getStateIdx() == State::DEAD || dynamic_cast<CUnit*>(pObject)->getStateIdx() == State::EVADE)
							return;

					pObject->hpChange(temp.nAtk, temp.type, temp.option);
					auto pEffect = (CEffect*)CObjectPool::getInst()->getObject(ObjectType::EFFECT,temp.nEffIdx);

					Vec2 zPos = pRoom->getObjectsList()[i]->getPosition();
					
					Vec2 yPos = Vec2(yBound.getMaxX(), yBound.getMinY());
					yPos = m_pOwner->convertToNodeSpace(yPos);
					yPos.x = 0.0f;

					pEffect->startEffects(zPos, yPos);
				}
			}
		}
	}
}

Rect CWeapon::getAttackBoxBounding()
{
	Rect rect = m_pAttackBox->getBoundingBox();

	Vec2 vPos = rect.origin;

	if (m_Dir.x < 0)
		vPos.x += rect.size.width * 0.5f;

	rect.origin = convertToWorldSpace(vPos);
	
	return rect;
}

void CWeapon::setUnitOwner(CUnit * pOwner)
{
	if (!m_pOwner)
	{
		m_pOwner = pOwner;
	}
}
