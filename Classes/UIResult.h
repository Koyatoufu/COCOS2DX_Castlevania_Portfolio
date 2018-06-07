#pragma once

#include "Includes.h"
#include "ui\CocosGUI.h"

using namespace ui;

class CUIResult :public Layer
{
private:
	Button * m_pBtnResult;
	Node * m_pRootNode;
	cocostudio::timeline::ActionTimeline *m_pTimeline;
	Text * m_pTextInfo;

	int m_nCurStage;
	int m_nInputState;
private:
	CUIResult();
	virtual ~CUIResult();
public:
	virtual bool init();
	
	CREATE_FUNC(CUIResult);
public:
	void playStageClear();
	void playGameOver();
private:
	void touchBtnResult(Ref * pSender, Widget::TouchEventType type);
};