#ifndef __CONVEY_PANEL_H__
#define __CONVEY_PANEL_H__

#include "SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class ConveyPanel : public SimplePanel
{	
public:
	ConveyPanel(std::string name):SimplePanel(name){}
	static ConveyPanel* create(std::string name);
	virtual bool init();
	void initWithUnlockMap(std::map<int, bool> map);
	void destroy();
private:
	Layout* _panelButton;
	Button* _closeButton;
private:
	void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__CONVEY_PANEL_H__*/