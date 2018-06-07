#pragma once

#include "Includes.h"

#include "ui\CocosGUI.h"

class CUsercreate :public Layer
{
private:
	Node * m_pRootNode;
	ui::TextBMFont* m_pMessageText;
	ui::TextField* m_pNameInput;
	ui::Button* m_pBtnOK;
private:
	CUsercreate();
	~CUsercreate();
public:
	virtual bool init();

	CREATE_FUNC(CUsercreate);

	void onTextCheck(Ref* pSender, ui::TextField::EventType eventType);
	void onTouchButton(Ref* pSender, ui::Button::TouchEventType eventType);
	void createUserData();
	void messageChange(const char* szStr);
};
