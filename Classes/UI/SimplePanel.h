#ifndef __SIMPLE_PANEL_H__
#define __SIMPLE_PANEL_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "cocostudio/CCActionManagerEx.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class SimplePanel : public Layer
{	
public:
	SimplePanel(std::string name);
	static SimplePanel* create(std::string name);
	virtual bool init();
	void onEnter();
	void onExit();
protected:
	Layout* _root;
	Layout* _container;
	Layout* _buttons;
	void destroy();
	std::string _url;
private:
	void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__SIMPLE_PANEL_H__*/