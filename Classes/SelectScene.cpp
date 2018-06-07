#include "SelectScene.h"

#include "StageScene.h"
#include "CharacterSelect.h"
#include "StageSelect.h"
#include "OptionSelect.h"

#include "DataMgr.h"
#include "SoundMgr.h"

#define USERINFO_FIELD 55
#define USERINFO_TEXT 57

CSelectScene::CSelectScene():m_pRootNode(0),m_pCreateLayer(0),m_pImgSelect(0),m_pTouchListner(0),m_pQuitMenu(0),m_pKeyListner(0)
{
}

CSelectScene::~CSelectScene()
{
}

Scene * CSelectScene::createScene()
{
	return CSelectScene::create();
}

bool CSelectScene::init()
{
	if (!Scene::init())
		return false;

	m_pRootNode = CSLoader::createNode("select/ui_select.csb");
	addChild(m_pRootNode);

	initWigets();

	CSoundMgr::getInst()->playBgm("sounds/bgm/select_main.mp3", true);

	return true;
}

void CSelectScene::onEnter()
{
	Scene::onEnter();

	m_pTouchListner = EventListenerTouchOneByOne::create();

	m_pTouchListner->onTouchBegan = CC_CALLBACK_2(CSelectScene::onTouchBegan, this);
	m_pTouchListner->onTouchEnded = CC_CALLBACK_2(CSelectScene::onTouchEnded,this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pTouchListner,this);

	m_pKeyListner = EventListenerKeyboard::create();
	m_pKeyListner->onKeyReleased = CC_CALLBACK_2(CSelectScene::onKeyRelease, this);
	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pKeyListner, this);
}

void CSelectScene::onExit()
{
	_eventDispatcher->removeEventListener(m_pTouchListner);
	_eventDispatcher->removeEventListener(m_pKeyListner);
	Scene::onExit();
}

void CSelectScene::initWigets()
{
	m_pBtns[(int)BtnSel_Idx::STAGE] = (Button*)m_pRootNode->getChildByTag((int)BtnSel::STAGE);
	m_pBtns[(int)BtnSel_Idx::CHARINFO] = (Button*)m_pRootNode->getChildByTag((int)BtnSel::CHARINFO);
	m_pBtns[(int)BtnSel_Idx::ACHIEVE] = (Button*)m_pRootNode->getChildByTag((int)BtnSel::ACHEIVE);
	m_pBtns[(int)BtnSel_Idx::OPTION] = (Button*)m_pRootNode->getChildByTag((int)BtnSel::OPTION);

	for (int i = 0; i < (int)BtnSel_Idx::MAX; i++)
	{
		m_pBtns[i]->addTouchEventListener(CC_CALLBACK_2(CSelectScene::btnSelectsT, this));
	}
	
	m_pImgSelect = (Sprite*)m_pRootNode->getChildByTag(SELECT_IMAGE);

	m_isInit = true;

	m_pInfoField = m_pRootNode->getChildByTag(USERINFO_FIELD);

	auto pUserData = CDataMgr::getInst()->getUserData();
	auto pText = (Text*)m_pInfoField->getChildByTag(USERINFO_TEXT);
	pText->setString(StringUtils::format("id:%d name:%s", pUserData->getId(),pUserData->getName().c_str()));

	if (CDataMgr::getInst()->getUserData()->getName().empty())
	{
		m_pCreateLayer = CUsercreate::create();
		addChild(m_pCreateLayer);
		m_isInit = false;

		for (int i = 0; i < (int)BtnSel_Idx::MAX; i++)
		{
			m_pBtns[i]->setVisible(false);
		}

		m_pInfoField->setVisible(false);
	}

	m_pQuitMenu = m_pRootNode->getChildByTag((int)QuitMenu::MENU);
	auto btnYes = (Button*)m_pQuitMenu->getChildByTag((int)QuitMenu::YES);
	btnYes->addTouchEventListener(CC_CALLBACK_2(CSelectScene::btnQuitYesNo,this));
	auto btnNo = (Button*)m_pQuitMenu->getChildByTag((int)QuitMenu::NO);
	btnNo->addTouchEventListener(CC_CALLBACK_2(CSelectScene::btnQuitYesNo, this));
}

void CSelectScene::ReInitWidgets(CUsercreate * pSender)
{
	if (!m_pCreateLayer)
		return;
	if (pSender != m_pCreateLayer)
		return;

	for (int i = 0; i < (int)BtnSel_Idx::MAX; i++)
	{
		m_pBtns[i]->setVisible(true);
	}

	m_pBtns[(int)BtnSel_Idx::ACHIEVE]->setVisible(false);

	m_isInit = true;

	m_pCreateLayer->removeFromParentAndCleanup(true);

	m_pInfoField->setVisible(true);

	auto pUserData = CDataMgr::getInst()->getUserData();
	auto pText = (Text*)m_pInfoField->getChildByTag(USERINFO_TEXT);
	pText->setString(StringUtils::format("id:%d name:%s", pUserData->getId(), pUserData->getName().c_str()));
}

void CSelectScene::onKeyRelease(EventKeyboard::KeyCode keyCode, Event * event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		if (!m_pQuitMenu)
			return;

		if (m_pQuitMenu->isVisible())
		{
			m_pQuitMenu->setVisible(false);
			for (int i = 0; i < (int)BtnSel_Idx::MAX; i++)
			{
				m_pBtns[i]->setVisible(true);
			}

			m_pBtns[(int)BtnSel_Idx::ACHIEVE]->setVisible(false);
			CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
			return;
		}

		m_pQuitMenu->setVisible(true);
		for (int i = 0; i < (int)BtnSel_Idx::MAX; i++)
		{
			m_pBtns[i]->setVisible(false);
		}
		CSoundMgr::getInst()->playEffect("sounds/effects/se_pause.ogg");
		//Director::getInstance()->pause();
	}
}

void CSelectScene::btnSelectsT(Ref * pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
		return;

	if (!m_isInit)
		return;

	m_pImgSelect->setVisible(true);

	int nTag = ((Node*)pSender)->getTag();

	std::string szTmp;
	switch ((BtnSel)nTag)
	{
	case BtnSel::STAGE:
		szTmp = "select/source/select_img_stage.png";
		break;
	case BtnSel::CHARINFO:
		szTmp = "select/source/select_img_char.png";
		break;
	case BtnSel::ACHEIVE:
		szTmp = "select/source/select_img_ach.png";
		break;
	case BtnSel::OPTION:
		szTmp = "select/source/select_img_opt.png";
		break;
	}

	m_pImgSelect->setTexture(szTmp);
	BtnSel* pTmp = new BtnSel();
	*pTmp = (BtnSel)nTag;
	m_pImgSelect->setUserData(pTmp);

	CSoundMgr::getInst()->playEffect("sounds/effects/btn_01.ogg");
}

void CSelectScene::btnQuitYesNo(Ref * pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
		return;

	Node* pNode = (Node*)pSender;
	QuitMenu tag = (QuitMenu)pNode->getTag();

	if (tag == QuitMenu::YES)
	{
		CSoundMgr::getInst()->playEffect("sounds/effects/se_door.ogg");
		Director::getInstance()->end();
		return;
	}
	else
	{
		m_pQuitMenu->setVisible(false);
		for (int i = 0; i < (int)BtnSel_Idx::MAX; i++)
		{
			m_pBtns[i]->setVisible(true);
		}

		m_pBtns[(int)BtnSel_Idx::ACHIEVE]->setVisible(false);
		CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
		return;
	}
}

bool CSelectScene::onTouchBegan(Touch * pTouch, Event * pEvent)
{
	if (!m_isInit)
		return false;

	if(!m_pImgSelect->getBoundingBox().containsPoint(pTouch->getLocation()))
	{
		return false;
	}

	return true;
}

void CSelectScene::onTouchEnded(Touch * pTouch, Event * pEvent)
{
	if (!m_pImgSelect->isVisible())
		return;

	if (!m_pImgSelect->getBoundingBox().containsPoint(pTouch->getLocation()))
		return;

	CSoundMgr::getInst()->playEffect("sounds/effects/btn_00.ogg");
	changeCurScene();
}

void CSelectScene::changeCurScene()
{
	BtnSel* pTmp = (BtnSel*)m_pImgSelect->getUserData();

	if (pTmp)
	{
		Scene* pScene = nullptr;

		switch (*pTmp)
		{
		case BtnSel::STAGE:
				pScene = CStageSelect::createScene();
			break;
		case BtnSel::CHARINFO:
				pScene = CCharcterSelect::createScene();
			break;
		case BtnSel::OPTION:
				pScene = COptionSelect::createScene();
			break;
		}

		if(pScene)
			Director::getInstance()->pushScene(pScene);
	}

	m_pImgSelect->setVisible(false);
	m_pImgSelect->setUserData(nullptr);

}
