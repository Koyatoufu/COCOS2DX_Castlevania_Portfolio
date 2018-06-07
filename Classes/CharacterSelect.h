#pragma once

#include "Includes.h"
#include "ui\CocosGUI.h"

using namespace ui;

class CCharcterSelect:public Scene
{
private:
	std::vector<Button*> m_charBtns;
	ImageView* m_pPortrait;
	Button* m_pBtnReturn;
	Node* m_pRootNode;
	Node* m_pInfo;
	TextBMFont* m_pMessageText;

	PLUnitIdx m_sels[2];
	Sprite*  m_arSelPic[2];
	int m_nCurIdx;
public:
	CCharcterSelect();
	virtual ~CCharcterSelect();
public:
	static Scene* createScene();
	CREATE_FUNC(CCharcterSelect);

	virtual bool init();
private:
	void initWidgets();

	void touchReturnBtn(Ref *pSender,Button::TouchEventType eventType);
	void touchCancelChar(Ref * pSender, Button::TouchEventType eventType);
	void touchOkBtn(Ref* pSender, Button::TouchEventType eventType);
	void touchCharBtns(Ref *pSender,Button::TouchEventType eventType);
};