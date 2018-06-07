#include "UIResult.h"
#include "DataMgr.h"
#include "StageMgr.h"
#include "SelectScene.h"
#include "SoundMgr.h"

#define RESULT_MENU 68

#define STAGE_CLEAR 1
#define GAMEOVER 0

CUIResult::CUIResult():m_pRootNode(0),m_pBtnResult(0),m_pTextInfo(0),m_nCurStage(-1),m_nInputState(-1)
{
}

CUIResult::~CUIResult()
{
}

bool CUIResult::init()
{
	if(!Layer::init())
		return false;

	m_pRootNode = CSLoader::createNode("stage/ui_result.csb");
	addChild(m_pRootNode);
	m_pTimeline = CSLoader::createTimeline("stage/ui_result.csb");
	m_pRootNode->runAction(m_pTimeline);

	Node* pMenu = m_pRootNode->getChildByTag(RESULT_MENU);

	m_pBtnResult = (Button*)pMenu->getChildByTag(BTN_RETURN);
	m_pBtnResult->addTouchEventListener(CC_CALLBACK_2(CUIResult::touchBtnResult,this));

	m_pTextInfo = (Text*)pMenu->getChildByTag(MESSAGE_TEXT);

	setVisible(false);

	return true;
}

void CUIResult::playStageClear()
{
	setVisible(true);

	m_pTimeline->play("stageclear", false);

	m_nInputState = STAGE_CLEAR;

	int nIdx = CStageMgr::getInst()->getCurStageNumber();

	if (nIdx == -1)
		return;

	auto pUserData = CDataMgr::getInst()->getUserData();
	auto pStageData = CDataMgr::getInst()->getStageData()->getStageData(nIdx);
	auto pObjData = CDataMgr::getInst()->getObjData();

	std::string str;

	if (pStageData.unlockChar != PLUnitIdx::NONE)
	{
		str = "you can go next stage";

		if (!pUserData->getCurGetChars()[(int)pStageData.unlockChar])
		{
			str = pObjData->getPlayerUnitData((unsigned int)(pStageData.unlockChar)).name;
			str = StringUtils::format("you save %s \n you can go next stage", str.c_str());
		}
	}
	else
	{
		str = "you can go next stage";
	}

	m_pTextInfo->setString(str);
}

void CUIResult::playGameOver()
{
	setVisible(true);

	m_pTimeline->play("gameover", false);

	m_nInputState = GAMEOVER;

	m_pTextInfo->setString("Return to main menu");
}

void CUIResult::touchBtnResult(Ref * pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
		return;

	if (m_nInputState == -1)
		return;

	if (m_nInputState == STAGE_CLEAR)
	{
		int nIdx = CStageMgr::getInst()->getCurStageNumber();

		if (nIdx == -1)
			return;

		auto pStageData = CDataMgr::getInst()->getStageData()->getStageData(nIdx);
		auto pUserData = CDataMgr::getInst()->getUserData();

		pUserData->setIsGetChar(true, (int)pStageData.unlockChar);

		for (unsigned int i = 0; i < pStageData.unlockStages.size(); i++)
		{
			pUserData->setIsUnlockStage(true, pStageData.unlockStages[i]);
		}
	}

	CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
	Director::getInstance()->replaceScene(CSelectScene::createScene());
}
