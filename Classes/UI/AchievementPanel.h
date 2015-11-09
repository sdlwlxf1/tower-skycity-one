#ifndef __ACHIEVEMENT_PANEL_H__
#define __ACHIEVEMENT_PANEL_H__

#include "UI/SimplePanel.h"
#include "UI/EndStatPanel.h"
#include "cell/Cell.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class AchievementPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1
	};
	AchievementPanel(std::string name):SimplePanel(name){}
	static AchievementPanel* create(std::string name);
	bool init();
	void initWithModel(std::vector<ValueMap> model);
private:
	Vector<EndStatPanel*> _endStatPanels;
	Vector<Widget*> _endStatUnits;
	ListView* _listView;
	Text* _endNumLab;
	Layout* _endStatUnit;
	std::vector<ValueMap> _model;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	virtual void onRemoveButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void selectedItemEvent(Ref* pSender, ListView::EventType type);
	void selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type);
};


#endif /*__ACHIEVEMENT_PANEL_H__*/