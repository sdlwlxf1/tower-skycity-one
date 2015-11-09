#ifndef __END_PANEL_H__
#define __END_PANEL_H__

#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class EndPanel : public SimplePanel
{	
public:
	enum buttons
	{
		BACK = 1,
		STAT
	};
	EndPanel(std::string name):SimplePanel(name){}
	static EndPanel* create(std::string name);
	bool init();
	void initWithNum(int id);
private:
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);

	Button* _statBtn;
};


#endif /*__END_PANEL_H__*/