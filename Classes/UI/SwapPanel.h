#ifndef __SWAP_PANEL_H__
#define __SWAP_PANEL_H__

#include "UI/SimplePanel.h"
#include "mutual/Swap.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class SwapPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1,
		BUY,
	};
	SwapPanel(std::string name):SimplePanel(name){}
	static SwapPanel* create(std::string name);
	bool init();
	void onEnter();
	void onExit();
	void destroy();
	void build(Swap* swap);
private:
	int _selected;
	void setSelected(int num);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void menuCallback(Ref* ref);
	Text* _title;
	Layout* _content;
	Layout* _introPan;
	Text* _introText;
	Swap* _swap;
	Menu* _menu;
};


#endif /*__SWAP_PANEL_H__*/