#ifndef __CONFIG_PANEL_H__
#define __CONFIG_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class ConfigPanel : public SimplePanel
{	
public:
	enum buttons
	{
		BG = 1,
		EFFECT,
		CLOSE
	};
	ConfigPanel(std::string name):SimplePanel(name){}
	static ConfigPanel* create(std::string name);
	bool init();
private:
	ImageView* _soundOffImg;
	ImageView* _soundOnImg;
	ImageView* _shakeOffImg;
	ImageView* _effectOnImg;
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__CONFIG_PANEL_H__*/