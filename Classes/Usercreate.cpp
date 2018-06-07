#include "Usercreate.h"
#include "SelectScene.h"
#include "DataMgr.h"

using namespace ui;

#define NAME_INPUT_FIELD 92
#define TEXT_LAYER 67
#define TEXT_FIELD 88
#define BTN_OK 93

#define DEFAULT_LOG_TEXT "Create User Infomation"

CUsercreate::CUsercreate():m_pRootNode(0),m_pMessageText(0),m_pNameInput(0),m_pBtnOK(0)
{
}

CUsercreate::~CUsercreate()
{
}

bool CUsercreate::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_pRootNode = CSLoader::createNode("select/usercreate.csb");
	addChild(m_pRootNode);

	m_pMessageText = (ui::TextBMFont*)m_pRootNode->getChildByTag(MESSAGE_TEXT);

	m_pNameInput = (ui::TextField*)m_pRootNode->getChildByTag(NAME_INPUT_FIELD)->getChildByTag(TEXT_LAYER)->getChildByTag(TEXT_FIELD);
	m_pNameInput->addEventListener(CC_CALLBACK_2(CUsercreate::onTextCheck, this));
	m_pNameInput->setMaxLength(8);
	m_pNameInput->setMaxLengthEnabled(true);

	m_pBtnOK = (ui::Button*)m_pRootNode->getChildByTag(NAME_INPUT_FIELD)->getChildByTag(BTN_OK);
	m_pBtnOK->addTouchEventListener(CC_CALLBACK_2(CUsercreate::onTouchButton,this));

	return true;
}

void CUsercreate::onTextCheck(Ref * pSender, ui::TextField::EventType eventType)
{
	TextField* pField = (TextField*)pSender;

	switch (eventType)
	{
	case TextField::EventType::ATTACH_WITH_IME:
		break;
	case TextField::EventType::DELETE_BACKWARD:
		break;
	case TextField::EventType::DETACH_WITH_IME:
		if (pField->getString().empty())
		{
			messageChange("Insert Your Name");
		}
		break;
	case TextField::EventType::INSERT_TEXT:
		if (pField->getString().length() >= 8)
		{
			messageChange("Don't Insert Word More");
			return;
		}
		break;
	}
}

void CUsercreate::onTouchButton(Ref * pSender, ui::Button::TouchEventType eventType)
{
	if (eventType != Button::TouchEventType::ENDED)
		return;

	if (m_pNameInput->getString().empty())
	{
		messageChange("No Inputs Words");
		return;
	}

	if (m_pNameInput->getString().length() < 4)
	{
		messageChange("Name Length Short then 4");
		m_pNameInput->setString("");
		return;
	}

	createUserData();

	auto pParent = (CSelectScene*)getParent();
	pParent->ReInitWidgets(this);
}

void CUsercreate::createUserData()
{
	CDataMgr* pDataMgr = CDataMgr::getInst();

	std::string name = m_pNameInput->getString();

	pDataMgr->getUserData()->initUserData(name);
}

void CUsercreate::messageChange(const char* szStr)
{
	m_pMessageText->runAction(Sequence::create(
		CallFunc::create([=]() { m_pMessageText->setString(szStr); }),
		DelayTime::create(0.5f),
		CallFunc::create([=]() { m_pMessageText->setString(DEFAULT_LOG_TEXT); }),
		nullptr));
}
