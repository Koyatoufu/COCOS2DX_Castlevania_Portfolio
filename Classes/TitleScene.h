#pragma once

#include "Includes.h"
using namespace cocostudio::timeline;

class CTitleScene :public cocos2d::Scene 
{
private:
	Node * m_pRootNode;
	ActionTimeline *m_pActTime;

	bool m_isTitleApear;
	EventListenerTouchOneByOne *m_pTouchListner;
	EventListenerKeyboard *m_pKeyListner;
	Node * m_QuitMenu;
public:
	static Scene* createScene();

	virtual bool init();

	CREATE_FUNC(CTitleScene);

	void onEnter();
	void onExit();
private:
	void titleIsDone(Frame* pFrame);

	bool onTouchBegin(Touch* pTouch, Event* pEvent);
	void onTouchEnded(Touch* pTouch, Event* pEvent);
	void onKeyRelease(EventKeyboard::KeyCode keyCode, Event* event);
	void btnYesTouch(Ref* pSender, ui::Widget::TouchEventType type);
	void btnNoTouch(Ref* pSender, ui::Widget::TouchEventType type);
};