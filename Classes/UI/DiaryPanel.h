#ifndef __DIARY_PANEL_H__
#define __DIARY_PANEL_H__

#include "UI/SimplePanel.h"
#include "TObject.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class Actor;

class DiaryPanel : public SimplePanel, public TObject
{	
public:
	enum buttons
	{
		EXIT = 0,
		TASK,
		TIP,
		MAP,
		LEFT,
		RIGHT
	};
	DiaryPanel(std::string name):SimplePanel(name){}
	DiaryPanel(std::string name, ValueMap model):SimplePanel(name){_model = model;}
	static DiaryPanel* create(std::string name);
	static DiaryPanel* create(std::string name, ValueMap model);
	bool init();
	void onEnter();
	void onExit();
	void addTask(std::string name);
	void addMap(std::string name);
	void addTip(std::string name);

	void initModel();
	void loadModel(ValueMap &map);
	ValueMap saveModel();
	bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();
	void setOwner(Actor* owner);
	Actor* getOwner();

	void show(std::string type, std::string name);
private:
	Actor* _owner;
	void destroy();
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	Layout* _selected;
	Layout* _task;
	Layout* _map;
	Layout* _tip;

	ValueMap _tasks;
	ValueMap _maps;
	ValueMap _tips;
};


#endif /*__DIARY_PANEL_H__*/