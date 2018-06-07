#pragma once

#include "Includes.h"
#include "ui\CocosGUI.h"

using namespace ui;

class CStageSelect:public Scene
{
private:
	Node * m_pRoot;
	std::vector<Button*> m_btnStages;
	Button * m_pBtnReturn;
	Text* m_pTextInfo;
	Sprite* m_pSprInfo;

	int m_nSelStage;
	EventListenerTouchOneByOne* m_pListener;
public:
	CStageSelect();
	virtual ~CStageSelect();
public:
	static Scene* createScene();

	virtual bool init();

	CREATE_FUNC(CStageSelect);
private:
	void initWidgets();
	void changeStageLogo(const char* szStr = "Select Stage");
	void goToStageScene();

	void touchBtnStages(Ref * pSender, Button::TouchEventType type);
	void touchReturn(Ref * pSender, Button::TouchEventType type);

	bool onTouchBegin(Touch * pTouch, Event * pEvent);
	void onTouchEnded(Touch * pTouch, Event * pEvent);
};