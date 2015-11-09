#ifndef __START_PANEL_H__
#define __START_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class StartPanel : public SimplePanel
{	
public:
	enum buttons
	{
		BEGIN = 1,
		OPTION,
		ACHIEVE,
		ABOUT
	};
	StartPanel(std::string name):SimplePanel(name){}
	static StartPanel* create(std::string name);
	bool init();
	void onEnter();
	void onExit();

	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
private:
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__START_PANEL_H__*/