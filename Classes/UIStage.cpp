#include "UIStage.h"
#include "PlayerMgr.h"
#include "SelectScene.h"
#include "DataMgr.h"
#include "SoundMgr.h"

bool CUIStage::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_pBase = CSLoader::getInstance()->createNode("stage/ui_stage.csb");
	addChild(m_pBase);

	initWigets();

	return true;
}

void CUIStage::onEnter()
{
	Layer::onEnter();

	auto pListener = EventListenerTouchAllAtOnce::create();

	pListener->onTouchesBegan = CC_CALLBACK_2(CUIStage::onTouchesBegin, this);
	pListener->onTouchesMoved = CC_CALLBACK_2(CUIStage::onTouchesMoved, this);
	pListener->onTouchesEnded = CC_CALLBACK_2(CUIStage::onTouchesEnded, this);
	pListener->onTouchesCancelled = nullptr;

	m_pListener = pListener;

	_eventDispatcher->addEventListenerWithSceneGraphPriority(pListener, this);
}

void CUIStage::onExit()
{
	_eventDispatcher->removeEventListener(m_pListener);

	Layer::onExit();
}

void CUIStage::settingGameOverMenu()
{
	if (!CPlayerMgr::getInst()->getCurrentPlayer()->getIsDead())
		return;


}

void CUIStage::changeSubweaponImg(unsigned int nIdx)
{
	auto data = CDataMgr::getInst()->getObjData()->getProjectileData(nIdx);

	m_pSubImg->setTexture(data.iconPath);
}

void CUIStage::initWigets()
{
	m_pPauseMenu = m_pBase->getChildByTag(PAUSE_MENU);

	auto pCase = m_pBase -> getChildByTag((int)StageBar::CASE);

	m_pHpBar = (LoadingBar*)pCase->getChildByTag((int)StageBar::HPBAR);
	m_pMpBar = (LoadingBar*)pCase->getChildByTag((int)StageBar::MPBAR);

	initButtons();

	m_pCharPortrait = (Sprite*)m_pBase->getChildByTag(CHAR_PORTRAIT)->getChildByTag(CHAR_PORTRAIT);
	auto pUserData = CDataMgr::getInst()->getUserData();
	auto unitData = CDataMgr::getInst()->getObjData()->getPlayerUnitData((unsigned int)pUserData->getUserPLIDX1());
	m_pCharPortrait->setTexture(StringUtils::format("hud/portrait/%s_portrait.png",unitData.name.c_str()));

	m_pPad = (Sprite*)m_pBase->getChildByTag(DPAD);
	m_pPadHwd = (Sprite*)m_pBase->getChildByTag(DPAD_HANDLE);

	m_pPad->setOpacity(70);
	m_pPadHwd->setOpacity(200);

	m_pPadHwd->setFlippedX(false);
	m_isPadTouch = false;;
}

void CUIStage::initButtons()
{
	ui::Button* pBtn = static_cast<ui::Button*>(m_pBase->getChildByTag((int)BtnStage::PAUSE));
	pBtn->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	m_pBtnPause = pBtn;

	{
		auto pBtnMenu = static_cast<ui::Button*>(m_pPauseMenu->getChildByTag((int)BtnStage::MENU));
		pBtnMenu->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));

		auto pBtnResume = static_cast<ui::Button*>(m_pPauseMenu->getChildByTag((int)BtnStage::RESUME));
		pBtnResume->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	}

	m_btnPlayers.reserve(6);
	
	Node* pTmp = m_pBase->getChildByTag((int)BtnStage::CHANGE);
	pBtn = static_cast<ui::Button*>(pTmp->getChildByTag((int)BtnStage::CHANGE));
	pBtn->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	m_btnPlayers.push_back(pBtn);

	pBtn = static_cast<ui::Button*>(m_pBase->getChildByTag((int)BtnStage::JUMP));
	pBtn->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	m_btnPlayers.push_back(pBtn);

	pBtn = static_cast<ui::Button*>(m_pBase->getChildByTag((int)BtnStage::EVADE));
	pBtn->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	m_btnPlayers.push_back(pBtn);

	pBtn = static_cast<ui::Button*>(m_pBase->getChildByTag((int)BtnStage::ATK));
	pBtn->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	m_btnPlayers.push_back(pBtn);

	pBtn = static_cast<ui::Button*>(m_pBase->getChildByTag((int)BtnStage::SUB));
	pBtn->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	m_pSubImg = (Sprite*)pBtn->getChildByTag((int)BtnStage::SUB);
	changeSubweaponImg(0);
	m_btnPlayers.push_back(pBtn);

	pBtn = static_cast<ui::Button*>(m_pBase->getChildByTag((int)BtnStage::SP));
	pBtn->addTouchEventListener(CC_CALLBACK_2(CUIStage::btnsTouch, this));
	m_btnPlayers.push_back(pBtn);
}

bool CUIStage::onTouchesBegin(const std::vector<Touch*> &touches, Event * pEvent)
{
	if (Director::getInstance()->isPaused())
		return false;

	CPlayer* pPlayer=CPlayerMgr::getInst()->getCurrentPlayer();

	if (pPlayer->getStateIdx() != State::IDLE && pPlayer->getStateIdx() != State::JUMP)
		return false;

	for (int i = 0; i < (int)touches.size(); i++)
	{
		Vec2 touchPos = touches[i]->getLocation();
		if (m_pPad->getBoundingBox().containsPoint(touchPos))
		{
			m_pPadHwd->setPosition(touchPos);
			Vec2 dis = touchPos - m_pPad->getPosition();
			dis.normalize();
			pPlayer->setDirection(dis);
			m_isPadTouch = true;
			bool isAir = pPlayer->getGData().isAir;
			if (!isAir)
				pPlayer->setState(State::MOVE);
		}
	}

	return true;
}

void CUIStage::onTouchesMoved(const std::vector<Touch*> &touches, Event * pEvent)
{
	for (int i = 0; i < (int)touches.size(); i++)
	{
		if (m_isPadTouch)
		{
			if (CPlayerMgr::getInst()->getCurrentPlayer()->getStateIdx() == State::IDLE)
				CPlayerMgr::getInst()->getCurrentPlayer()->setState(State::MOVE);

			Vec2 touchPos = touches[i]->getLocation();
			Vec2 dis = touchPos;

			if (!m_pPad->getBoundingBox().containsPoint(touchPos))
			{
				dis = touchPos - m_pPad->getPosition();

				dis.normalize();

				float fLength = m_pPad->getContentSize().width * 0.5f;
				dis = dis * fLength;
				dis = m_pPad->getPosition() + dis;
			}
			m_pPadHwd->setPosition(dis);

			dis = dis - m_pPad->getPosition();

			dis = dis * 0.05f;

			CPlayerMgr::getInst()->getCurrentPlayer()->setDirection(dis);
		}
	}
}

void CUIStage::onTouchesEnded(const std::vector<Touch*> &touches, Event * pEvent)
{
	if (m_isPadTouch)
	{
		Vec2 dis = m_pPadHwd->getPosition();
		m_pPadHwd->setPosition(m_pPad->getPosition());
		m_isPadTouch = false;

		if (!CPlayerMgr::getInst()->getCurrentPlayer()->getGData().isAir)
		{
			dis -= m_pPadHwd->getPosition();
			dis.normalize();
			CPlayerMgr::getInst()->getCurrentPlayer()->setDirection(dis);
		}
	}
}

void CUIStage::update(float fDelta)
{
	CPlayer* pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();

	if (!pPlayer)
		return;

	ObjHpData hpData = pPlayer->getHpData();

	float fHpPercent = hpData.fCurHp / hpData.fMaxHp*100.0f;
	float fMpPercent = hpData.fCurMp / hpData.fMaxMp*100.0f;

	m_pHpBar->setPercent(fHpPercent);
	m_pMpBar->setPercent(fMpPercent);
}

void CUIStage::btnsTouch(Ref * pSender, ui::Widget::TouchEventType type)
{
	Node *pTmp = (Node*)pSender;
	BtnStage nTag = (BtnStage)pTmp->getTag();

	CPlayer* pPlayer = CPlayerMgr::getInst()->getCurrentPlayer();

	if (!pPlayer)
		return;

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (nTag)
		{
		case BtnStage::PAUSE:
			m_pPauseMenu->setVisible(!m_pPauseMenu->isVisible());
			
			if (!Director::getInstance()->isPaused())
				Director::getInstance()->pause();
			pTmp->setVisible(false);
			CSoundMgr::getInst()->playEffect("sounds/effects/se_pause.ogg");
			return;
		case BtnStage::RESUME:
			m_pPauseMenu->setVisible(!m_pPauseMenu->isVisible());
			if (Director::getInstance()->isPaused())
				Director::getInstance()->resume();
			m_pBtnPause->setVisible(true);
			CSoundMgr::getInst()->playEffect("sounds/effects/btn_01.ogg");
			return;
		case BtnStage::MENU:
			if (Director::getInstance()->isPaused())
				Director::getInstance()->resume();
			Director::getInstance()->replaceScene(CSelectScene::createScene());
			CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
			return;
		}
		
		if (pPlayer->getStateIdx() == State::DEAD)
			return;

		switch (nTag)
		{
		case BtnStage::JUMP:
			if (!pPlayer->getGData().isAir && pPlayer->getStateIdx() != State::EVADE)
				pPlayer->setState(State::JUMP);
			return;
		case BtnStage::EVADE:
			if (!pPlayer->getGData().isAir && pPlayer->getStateIdx() != State::EVADE)
				pPlayer->setState(State::EVADE);
			break;
		case BtnStage::ATK:
			pPlayer->setState(State::ATTACK);
			break;
		case BtnStage::SUB:
			pPlayer->setState(State::SUBATTACK);
			break;
		case BtnStage::SP:
			if (pPlayer->getStateIdx() == State::SKILL || pPlayer->getStateIdx() == State::EVADE || pPlayer->getGData().isAir)
				break;

			pPlayer->setState(State::SKILL);
			break;
		}
	}
}
