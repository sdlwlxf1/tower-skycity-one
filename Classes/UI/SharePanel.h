#ifndef __START_PANEL_H__
#define __START_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class SharePanel : public SimplePanel
{	
public:
	enum buttons
	{
		BEGIN = 1,
	};
	SharePanel(std::string name):SimplePanel(name){}
	static SharePanel* create(std::string name);
	bool init();
	void onEnter();
	void onExit();

	void setInfo(std::string info);
private:
	Layout* _shareWaysPan;
	std::string _info;
	//void setSelectShareWay(int index);
	//void onSelectShareWays(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	//void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	//void onAuth(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onShare(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__START_PANEL_H__*/