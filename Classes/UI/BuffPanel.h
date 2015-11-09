#ifndef __BUFF_PANEL_H__
#define __BUFF_PANEL_H__

#include "UI/SimplePanel.h"
#include "buff/Buff.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class BuffPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1
	};
	BuffPanel(std::string name):SimplePanel(name){}
	static BuffPanel* create(std::string name);
	bool init();
	void initWithBuff(Buff* buff);
	void updateBuffTime(double time);

	CC_SYNTHESIZE(Buff*, _buff, Buff);
	void destroy();
private:
	
	Text* _nameLab;
	Text* _introLab;
	Text* _remTimeLab;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__BUFF_PANEL_H__*/