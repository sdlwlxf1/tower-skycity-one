#ifndef __PAPER_PANEL_H__
#define __PAPER_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class PaperPanel : public SimplePanel
{	
public:
	enum buttons
	{
	};
	PaperPanel(std::string name):SimplePanel(name){}
	static PaperPanel* create(std::string name);
	bool init();
	void initWithText(std::string text);
	void initWithTextNum(int num);
private:
	ScrollView* _scrollView;
	Label* _label;
	Label* _title;
	int _textId;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__PAPER_PANEL_H__*/