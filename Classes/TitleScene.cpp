#include "TitleScene.h"
#include "ui/UIButton.h"
#include "SelectScene.h"
#include "DataMgr.h"
#include "SoundMgr.h"

Scene * CTitleScene::createScene()
{
	return CTitleScene::create();
}

bool CTitleScene::init()
{
	if(!Scene::init())
		return false;
	
	m_pRootNode = CSLoader::createNode("title/ui_title.csb");
	addChild(m_pRootNode);

	m_QuitMenu = m_pRootNode->getChildByTag((int)QuitMenu::MENU);

	auto pBtnYes = (ui::Button*)m_QuitMenu->getChildByTag((int)QuitMenu::YES);
	pBtnYes->addTouchEventListener(CC_CALLBACK_2(CTitleScene::btnYesTouch, this));

	auto pBtnNo=(ui::Button*)m_QuitMenu->getChildByTag((int)QuitMenu::NO);
	pBtnNo->addTouchEventListener(CC_CALLBACK_2(CTitleScene::btnNoTouch, this));

	m_pActTime = CSLoader::createTimeline("title/ui_title.csb");
	m_pActTime->retain();

	auto pAction = m_pActTime->clone();

	pAction->setFrameEventCallFunc(CC_CALLBACK_1(CTitleScene::titleIsDone,this));

	m_pRootNode->runAction(pAction);
	pAction->play("TitleApear", false);
	CSoundMgr::getInst()->playBgm("sounds/bgm/title.mp3", true);

	m_isTitleApear = false;

	return true;
}

void CTitleScene::onEnter()
{
	Scene::onEnter();

	m_pTouchListner = EventListenerTouchOneByOne::create();

	m_pTouchListner->onTouchBegan = CC_CALLBACK_2(CTitleScene::onTouchBegin,this);
	m_pTouchListner->onTouchEnded = CC_CALLBACK_2(CTitleScene::onTouchEnded, this);

	m_pKeyListner = EventListenerKeyboard::create();

	m_pKeyListner->onKeyReleased = CC_CALLBACK_2(CTitleScene::onKeyRelease,this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pTouchListner, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pKeyListner, this);
}

void CTitleScene::onExit()
{
	_eventDispatcher->removeEventListener(m_pTouchListner);
	_eventDispatcher->removeEventListener(m_pKeyListner);

	Scene::onExit();
}

void CTitleScene::titleIsDone(Frame * pFrame)
{
	auto pEventFrame = (EventFrame*)pFrame;
	if (!pEventFrame)
		return;

	std::string szStr = pEventFrame->getEvent();

	if (szStr == "TitleDone")
	{
		auto pAction = m_pActTime->clone();
		pAction->play("FadeInOut", true);

		m_pRootNode->runAction(pAction);

		m_isTitleApear = true;
	}
}

bool CTitleScene::onTouchBegin(Touch * pTouch, Event * pEvent)
{
	if(!m_isTitleApear)
		return false;
	if (m_QuitMenu->isVisible())
		return false;

	return true;
}

void CTitleScene::onTouchEnded(Touch * pTouch, Event * pEvent)
{
	CSoundMgr::getInst()->playEffect("sounds/effects/se_door.ogg");
	Director::getInstance()->replaceScene(CSelectScene::createScene());
}

void CTitleScene::onKeyRelease(EventKeyboard::KeyCode keyCode, Event * event)
{
	if (!m_isTitleApear)
		return;

	if (keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		if (m_QuitMenu->isVisible())
		{
			m_QuitMenu->setVisible(false);
			Director::getInstance()->resume();
			CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
			return;
		}

		m_QuitMenu->setVisible(true);
		CSoundMgr::getInst()->playEffect("sounds/effects/se_pause.ogg");
		Director::getInstance()->pause();
	}
}

void CTitleScene::btnYesTouch(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (type != ui::Widget::TouchEventType::BEGAN)
		return;

	CSoundMgr::getInst()->playEffect("sounds/effects/se_door.ogg");
	Director::getInstance()->end();
}

void CTitleScene::btnNoTouch(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (type != ui::Widget::TouchEventType::BEGAN)
		return;

	m_QuitMenu->setVisible(false);

	CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
	Director::getInstance()->resume();
}

//auto sprite = Sprite::createWithSpriteFrameName("sprite-frame-name.png");
//ActionTimeline *actionTimeline = CSLoader::createTimeline("CocoStudioTimeLine.csb");
//actionTimeline->retain();
//_rootNode->addChild(sprite);

//sprite->runAction(actionTimeline);
//Timeline *currentTimeLine = actionTimeline->getTimelines().at(0); //access Timeline from Vector
//currentTimeLine->setNode(sprite);
//actionTimeline->play("animation_name", true);