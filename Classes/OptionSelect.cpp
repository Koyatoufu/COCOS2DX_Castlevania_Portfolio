#include "OptionSelect.h"
#include "SoundMgr.h"

#define OPTION_MENU 57
#define SLIDER_BGM 3
#define SLIDER_SE 4
#define BTN_CONFIRM 58

COptionSelect::COptionSelect()
{
}

COptionSelect::~COptionSelect()
{
}

Scene * COptionSelect::createScene()
{
	return COptionSelect::create();
}

bool COptionSelect::init()
{
	if(!Scene::init())
		return false;

	m_pRoot = CSLoader::createNode("select/option_select.csb");
	addChild(m_pRoot);

	m_pBgmSlider = (Slider*)m_pRoot->getChildByTag(OPTION_MENU)->getChildByTag(SLIDER_BGM);
	m_pBgmSlider->addTouchEventListener(CC_CALLBACK_2(COptionSelect::touchSlider, this));
	m_pBgmSlider->setPercent(CSoundMgr::getInst()->getBgmVolume() * 100);

	m_pSESlider = (Slider*)m_pRoot->getChildByTag(OPTION_MENU)->getChildByTag(SLIDER_SE);
	m_pSESlider->addTouchEventListener(CC_CALLBACK_2(COptionSelect::touchSlider, this));
	m_pSESlider->setPercent(CSoundMgr::getInst()->getEffectVolume() * 100);

	m_pBtnConfirm = (Button*)m_pRoot->getChildByTag(OPTION_MENU)->getChildByTag(BTN_CONFIRM);
	m_pBtnConfirm->addTouchEventListener(CC_CALLBACK_2(COptionSelect::touchConfirm, this));

	CSoundMgr::getInst()->playBgm("sounds/bgm/select_option.mp3", true);

	return true;
}

void COptionSelect::touchSlider(Ref * pRef, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::CANCELED || type == Widget::TouchEventType::BEGAN)
		return;

	Slider* pSlider = (Slider*)pRef;

	if (pSlider == m_pBgmSlider)
	{
		m_nBgmPercent = m_pBgmSlider->getPercent();
		CSoundMgr::getInst()->setBgmVolume(m_nBgmPercent*0.01f);
	}
	else if (pSlider == m_pSESlider)
	{
		m_nSePercent = m_pSESlider->getPercent();
		CSoundMgr::getInst()->setEffectsVolume(m_nSePercent*0.01f);
	}

	CSoundMgr::getInst()->playEffect("sounds/effects/btn_02.ogg");
}

void COptionSelect::touchConfirm(Ref * pRef, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
		return;

	Director::getInstance()->popScene();
	CSoundMgr::getInst()->playEffect("sounds/effects/btn_00.ogg");
	CSoundMgr::getInst()->playBgm("sounds/bgm/select_main.mp3", true);
}
