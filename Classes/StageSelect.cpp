#include "StageSelect.h"
#include "StageScene.h"
#include "DataMgr.h"
#include "SoundMgr.h"

CStageSelect::CStageSelect():m_pRoot(0),m_pBtnReturn(0),m_nSelStage(-1),m_pListener(0),m_pTextInfo(0),m_pSprInfo(0)
{
}

CStageSelect::~CStageSelect()
{
}

Scene * CStageSelect::createScene()
{
	auto pScene = CStageSelect::create();

	return pScene;
}

bool CStageSelect::init()
{
	if (!Scene::init())
		return false;

	m_pRoot = CSLoader::createNode("select/stage_select.csb");
	addChild(m_pRoot);

	m_pListener = EventListenerTouchOneByOne::create();
	m_pListener->onTouchBegan = CC_CALLBACK_2(CStageSelect::onTouchBegin,this);
	m_pListener->onTouchEnded = CC_CALLBACK_2(CStageSelect::onTouchEnded,this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pListener, this);

	initWidgets();

	CSoundMgr::getInst()->playBgm("sounds/bgm/select_stage.mp3", true);

	return true;
}

void CStageSelect::initWidgets()
{
	auto pUserData = CDataMgr::getInst()->getUserData();
	
	const std::vector<bool>& curUnlocked = pUserData->getCurUnlockStages();

	m_btnStages.reserve(curUnlocked.capacity());
	m_btnStages.resize(curUnlocked.size());

	for (unsigned int i = 0; i < curUnlocked.size(); i++)
	{
		m_btnStages[i] = (Button*)m_pRoot->getChildByTag(41)->getChildByTag(i);
		m_btnStages[i]->addTouchEventListener(CC_CALLBACK_2(CStageSelect::touchBtnStages, this));

		if (curUnlocked[i])
		{
			m_btnStages[i]->setEnabled(true);
		}
	}

	m_pSprInfo = (Sprite*)m_pRoot->getChildByTag(42);
	m_pTextInfo = (Text*)m_pSprInfo->getChildByTag(55);

	m_pBtnReturn = (Button*)m_pRoot->getChildByTag(BTN_RETURN);
	m_pBtnReturn->addTouchEventListener(CC_CALLBACK_2(CStageSelect::touchReturn, this));
}

void CStageSelect::changeStageLogo(const char * szStr)
{
	if (szStr == "Select Stage")
	{
		m_nSelStage = -1;
		CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
	}
	m_pTextInfo->setString(szStr);
}

void CStageSelect::goToStageScene()
{
	if (m_nSelStage == -1)
		return;

	auto pStageData = CDataMgr::getInst()->getStageData();
	auto pUserData = CDataMgr::getInst()->getUserData();

	Director::getInstance()->popScene();
	auto pScene = CStageScene::createScene(m_nSelStage);
	Director::getInstance()->replaceScene(pScene);
}

void CStageSelect::touchBtnStages(Ref * pSender, Button::TouchEventType type)
{
	if (type != Button::TouchEventType::ENDED)
		return;

	Node* pNode = (Node*)pSender;

	int nIdx = pNode->getTag();

	m_nSelStage = nIdx;

	auto pStageData = CDataMgr::getInst()->getStageData();

	changeStageLogo(pStageData->getStageData(nIdx).name.c_str());
	CSoundMgr::getInst()->playEffect("sounds/effects/btn_01.ogg");
}

void CStageSelect::touchReturn(Ref * pSender, Button::TouchEventType type)
{
	if (type != Button::TouchEventType::ENDED)
		return;

	Director::getInstance()->popScene();
	CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
	CSoundMgr::getInst()->playBgm("sounds/bgm/select_main.mp3", true);
}

bool CStageSelect::onTouchBegin(Touch * pTouch, Event * pEvent)
{
	Vec2 pos = pTouch->getLocation();

	if (m_pSprInfo->getBoundingBox().containsPoint(pos))
		return true;

	changeStageLogo();
	return false;
}

void CStageSelect::onTouchEnded(Touch * pTouch, Event * pEvent)
{
	Vec2 pos = pTouch->getLocation();

	if (m_pSprInfo->getBoundingBox().containsPoint(pos))
	{
		if (m_nSelStage != -1)
		{
			CSoundMgr::getInst()->playEffect("sounds/effects/btn_00.ogg");
			goToStageScene();
			return;
		}
	}

	changeStageLogo();
}
