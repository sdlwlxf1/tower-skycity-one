#ifndef __ENDSTAT_PANEL_H__
#define __ENDSTAT_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class EndStatPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1
	};
	EndStatPanel(std::string name):SimplePanel(name){}
	static EndStatPanel* create(std::string name);
	bool init();
	void initWithModel(ValueMap model);
private:
	Text* _labelHp;
	Text* _labelStr;
	Text* _labelDef;
	Text* _labelGold;
	Text* _labelXp;
	Text* _labelHoeNum;
	Text* _labelBombNum;
	Text* _labelTime;
	Text* _labelEndTag;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__ENDSTAT_PANEL_H__*/