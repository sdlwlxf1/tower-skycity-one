#ifndef __LEVEL_UP_PANEL_H__
#define __LEVEL_UP_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class LevelUpPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1
	};
	LevelUpPanel(std::string name):SimplePanel(name){}
	static LevelUpPanel* create(std::string name);
	bool init();
	void initWithNums(int nums[]);
private:
	Text* _labelAddHp;
	Text* _labelAddStr;
	Text* _labelAddDef;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__LEVEL_UP_PANEL_H__*/