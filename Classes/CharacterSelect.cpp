#include "CharacterSelect.h"
#include "DataMgr.h"
#include "SoundMgr.h"

#define MAX_CHAR_SEL 1
#define PORTRAIT_IMG 18
#define PORTRAIT_FIRST 27
#define PORTRAIT_SECOND 28
#define INFO_LAYER 72
#define CHARSEL_BTN_OK 31

CCharcterSelect::CCharcterSelect():m_pRootNode(0), m_charBtns(), m_pMessageText(0), m_pBtnReturn(0),m_pInfo(0),m_nCurIdx(-1)
{
	m_sels[0] = PLUnitIdx::NONE;
	m_sels[1] = PLUnitIdx::NONE;
}

CCharcterSelect::~CCharcterSelect()
{
}

Scene * CCharcterSelect::createScene()
{
	auto pScene = CCharcterSelect::create();

	return pScene;
}

bool CCharcterSelect::init()
{
	if (!Scene::init())
		return false;

	m_pRootNode = CSLoader::createNode("select/character_select.csb");
	addChild(m_pRootNode);

	initWidgets();

	CSoundMgr::getInst()->playBgm("sounds/bgm/select_char.mp3", true);

	return true;
}

void CCharcterSelect::initWidgets()
{
	auto pDataMgr = CDataMgr::getInst();

	auto pUserData = pDataMgr->getUserData();
	auto pObjData = pDataMgr->getObjData();

	auto pBtnNode = m_pRootNode->getChildByTag(73);

	m_pBtnReturn = (Button*)m_pRootNode->getChildByTag(BTN_RETURN);
	m_pBtnReturn->addTouchEventListener(CC_CALLBACK_2(CCharcterSelect::touchReturnBtn,this));

	m_charBtns.reserve(pUserData->getCurGetChars().capacity());
	m_charBtns.resize(pUserData->getCurGetChars().size());

	for (int i = 0; i < pUserData->getCurGetChars().size(); i++)
	{
		m_charBtns[i] = (Button*)pBtnNode->getChildByTag(i);
		m_charBtns[i]->addTouchEventListener(CC_CALLBACK_2(CCharcterSelect::touchCharBtns,this));

		if (pUserData->getCurGetChars()[i])
		{
			m_charBtns[i]->setEnabled(true);
			Sprite* pSpr = (Sprite*)m_charBtns[i]->getChildByTag(CHAR_PORTRAIT);

			std::string str = pObjData->getPlayerUnitData(i).name;
			str = StringUtils::format("hud/portrait/%s_portrait.png", str.c_str());
			pSpr->setTexture(str);
			pSpr->setColor(Color3B::WHITE);
		}
	}

	m_pMessageText = (TextBMFont*)m_pRootNode->getChildByTag(MESSAGE_TEXT);
	m_pMessageText->setString("Select Character");

	m_pInfo = m_pRootNode->getChildByTag(INFO_LAYER);

	auto pBtnOk = (Button*)m_pInfo->getChildByTag(CHARSEL_BTN_OK);
	pBtnOk->addTouchEventListener(CC_CALLBACK_2(CCharcterSelect::touchOkBtn, this));

	m_arSelPic[0] = (Sprite*)m_pInfo->getChildByTag(PORTRAIT_FIRST);
	m_arSelPic[1] = (Sprite*)m_pInfo->getChildByTag(PORTRAIT_SECOND);

	m_pPortrait = (ImageView*)m_pRootNode->getChildByTag(PORTRAIT_IMG);
	m_pPortrait->addTouchEventListener(CC_CALLBACK_2(CCharcterSelect::touchCancelChar, this));
}

void CCharcterSelect::touchReturnBtn(Ref * pSender, Button::TouchEventType eventType)
{
	Director::getInstance()->popScene();
	CSoundMgr::getInst()->playBgm("sounds/bgm/select_main.mp3", true);
	CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
}

void CCharcterSelect::touchCancelChar(Ref * pSender, Button::TouchEventType eventType)
{
	if (eventType != Button::TouchEventType::ENDED)
		return;

	m_sels[m_nCurIdx] = PLUnitIdx::NONE;

	std::string str = "hud/portrait/none_portrait.png";

	m_arSelPic[m_nCurIdx]->setTexture(str);

	m_nCurIdx--;

	if (m_nCurIdx == -1)
	{
		m_pMessageText->setString("Select Character");
		m_pPortrait->setVisible(false);
		m_pPortrait->setTouchEnabled(false);
		m_pInfo->setVisible(false);
	}
	else
	{
		auto pUnitData = CDataMgr::getInst()->getObjData()->getPlayerUnitData((unsigned int)m_sels[m_nCurIdx]);
		
		m_pMessageText->setString(pUnitData.name);
		std::string str = StringUtils::format("select/source/charsel_port_%s.png", pUnitData.name.c_str());

		m_pPortrait->loadTexture(str);
	}

	CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
}

void CCharcterSelect::touchOkBtn(Ref * pSender, Button::TouchEventType eventType)
{
	if (eventType != Button::TouchEventType::ENDED)
		return;

	if (m_nCurIdx == -1)
		return;
	
	auto pUserData = CDataMgr::getInst()->getUserData();

	pUserData->setUserPLIDXs(m_sels[0], m_sels[1]);

	Director::getInstance()->popScene();
	CSoundMgr::getInst()->playBgm("sounds/bgm/select_main.mp3", true);
	CSoundMgr::getInst()->playEffect("sounds/effects/btn_00.ogg");
}

void CCharcterSelect::touchCharBtns(Ref * pSender, Button::TouchEventType eventType)
{
	if (eventType != Button::TouchEventType::ENDED)
		return;

	if (m_nCurIdx >= MAX_CHAR_SEL)
		return;

	auto pNode = (Node*)pSender;

	auto pUnitData = CDataMgr::getInst()->getObjData()->getPlayerUnitData((unsigned int)pNode->getTag());

	m_nCurIdx++;

	for (int i = 0; i < 2; i++)
	{
		if (m_sels[i] == (PLUnitIdx)pNode->getTag())
		{
			m_nCurIdx--;
			return;
		}
	}

	m_sels[m_nCurIdx] = (PLUnitIdx)pNode->getTag();
	m_pMessageText->setString(pUnitData.name);
	m_pInfo->setVisible(true);
	m_pPortrait->setVisible(true);
	m_pPortrait->setTouchEnabled(true);

	std::string str;

	str = StringUtils::format("hud/portrait/%s_portrait.png", pUnitData.name.c_str());

	m_arSelPic[m_nCurIdx]->setTexture(str);

	str = StringUtils::format("select/source/charsel_port_%s.png", pUnitData.name.c_str());
	
	m_pPortrait->loadTexture(str);

	CSoundMgr::getInst()->playEffect("sounds/effects/btn_01.ogg");
}