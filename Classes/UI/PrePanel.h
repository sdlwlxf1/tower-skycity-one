#ifndef __PRE_PANEL_H__
#define __PRE_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class PrePanel : public SimplePanel
{	
public:
	enum buttons
	{
		BEGIN = 1,
		OPTION,
		ABOUT
	};
	PrePanel(std::string name):SimplePanel(name){}
	static PrePanel* create(std::string name);
	bool init();
	void onEnter();
	void onExit();
	void rollText(float);
private:
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	Sprite* _image;
	Label* _text;
	ClippingNode* _cliper;
};

class PageTransitionForward : public TransitionPageTurn
{
public:
	static TransitionScene* create(float t, Scene* s)
	{
		Director::getInstance()->setDepthTest(true);
		return TransitionPageTurn::create(t, s, false);
	}
};

class PageTransitionBackward : public TransitionPageTurn
{
public:
	static TransitionScene* create(float t, Scene* s)
	{
		Director::getInstance()->setDepthTest(true);
		return TransitionPageTurn::create(t, s, true);
	}
};


#endif /*__PRE_PANEL_H__*/