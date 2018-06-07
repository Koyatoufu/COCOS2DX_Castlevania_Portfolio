#pragma once

#include "Includes.h"
#include "ui\CocosGUI.h"

using namespace ui;

class CUIStage :public Layer
{
private:
	Node * m_pBase;
	EventListener *m_pListener;
private:
	bool m_isPadTouch;

	Sprite *m_pPad;
	Sprite *m_pPadHwd;
	Sprite *m_pCharPortrait;
	Sprite *m_pSubImg;
	Node *m_pPauseMenu;

	LoadingBar *m_pHpBar;
	LoadingBar *m_pMpBar;

	Button *m_pBtnPause;
	std::vector<Button*> m_btnPlayers;
public:
	virtual bool init();

	CREATE_FUNC(CUIStage);

	void onEnter();
	void onExit();
	void update(float fDelta);
public:
	void settingGameOverMenu();
	void changeSubweaponImg(unsigned int nIdx);
private:
	bool onTouchesBegin(const std::vector<Touch*> &touches, Event* pEvent);
	void onTouchesMoved(const std::vector<Touch*> &touches, Event* pEvent);
	void onTouchesEnded(const std::vector<Touch*> &touches, Event* pEvent);
private:
	void initButtons();
	void initWigets();

	void btnsTouch(Ref* pSender, ui::Widget::TouchEventType type);
};

