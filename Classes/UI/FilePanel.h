#ifndef __FILE_PANEL_H__
#define __FILE_PANEL_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class FileUnit : public Ref
{
public:
	static FileUnit* create(int id);
	FileUnit(int id);
	void setView(Widget* view);
	void updateView();
	void save();
	void load();
	void clear();
	CC_SYNTHESIZE(bool, _isExist, IsExist);
private:
	int _id;
	std::string _path;
	Widget* _view;
	void saveModel();
};

class FilePanel : public SimplePanel
{	
public:
	enum buttons
	{
		CLOSE = 1,
		RESTART
	};
	enum func
	{
		SAVE = 1,
		LOAD,
	};
	FilePanel(std::string name):SimplePanel(name){}
	static FilePanel* create(std::string name);
	bool init();
	void onEnter();
	void setType(func type);
private:
	void onFileUnitsClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	Vector<FileUnit*> _units;
	func _type;
};


#endif /*__FILE_PANEL_H__*/