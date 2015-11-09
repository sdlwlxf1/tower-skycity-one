#ifndef __PAUSE_PANEL_H__
#define __PAUSE_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class PausePanel : public SimplePanel
{	
public:
	enum buttons
	{
		CONTINUE = 1,
		SAVE,
		ABOUT,
		BACK
	};
	PausePanel(std::string name):SimplePanel(name){}
	static PausePanel* create(std::string name);
	bool init();
private:
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__PAUSE_PANEL_H__*/