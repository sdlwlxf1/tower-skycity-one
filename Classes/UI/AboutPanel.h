#ifndef __ABOUT_PANEL_H__
#define __ABOUT_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class AboutPanel : public SimplePanel
{	
public:
	enum buttons
	{
	};
	AboutPanel(std::string name):SimplePanel(name){}
	static AboutPanel* create(std::string name);
	bool init();
private:
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__ABOUT_PANEL_H__*/