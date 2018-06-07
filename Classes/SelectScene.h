#pragma once

#include "Usercreate.h"

using namespace ui;

class CSelectScene :public Scene
{
private:
	Node * m_pRootNode;
	
	Button *m_pBtns[(int)BtnSel_Idx::MAX];
	Sprite* m_pImgSelect;
	EventListenerTouchOneByOne *m_pTouchListner;
	EventListenerKeyboard * m_pKeyListner;
	CUsercreate* m_pCreateLayer;
	Node * m_pInfoField;
	Node * m_pQuitMenu;
	bool m_isInit;
private:
	CSelectScene();
	virtual ~CSelectScene();
public:
	static Scene* createScene();

	virtual bool init();
	
	CREATE_FUNC(CSelectScene);

	void onEnter();
	void onExit();
public:
	void ReInitWidgets(CUsercreate* pSender);
private:
	void initWigets();

	void onKeyRelease(EventKeyboard::KeyCode keyCode, Event * event);
	void btnSelectsT(Ref* pSender, Widget::TouchEventType type);
	void btnQuitYesNo(Ref* pSender, Widget::TouchEventType type);
	bool onTouchBegan(Touch *pTouch, Event *pEvent);
	void onTouchEnded(Touch *pTouch, Event *pEvent);
	void changeCurScene();
};
