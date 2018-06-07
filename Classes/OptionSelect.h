#pragma once

#include "Includes.h"
#include "ui\CocosGUI.h"

using namespace ui;

class COptionSelect :public Scene
{
private:
	Node * m_pRoot;
	
	Slider* m_pBgmSlider;
	Slider* m_pSESlider;
	Button* m_pBtnConfirm;

	int m_nSePercent;
	int m_nBgmPercent;
private:
	COptionSelect();
	virtual ~COptionSelect();
public:
	static Scene* createScene();

	virtual bool init();

	CREATE_FUNC(COptionSelect);
private:
	void touchSlider(Ref* pRef, Widget::TouchEventType type);
	void touchConfirm(Ref* pRef, Widget::TouchEventType type);
};