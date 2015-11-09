#ifndef __SHOP_PANEL_H__
#define __SHOP_PANEL_H__

#include "UI/SimplePanel.h"
#include "cell/Cell.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class GoodUnit : public Ref
{
public:
	static GoodUnit* create(int id);
	GoodUnit(int id);
	void setView(Widget* view);
	void updateView();
	int getNum(){return _num;}
	void setNum(int num){_num = num;}
	void setGood(Cell* good);
	Cell* getGood(){return _good;}
	CC_SYNTHESIZE(int, _price, Price);
	~GoodUnit();
private:
	int _num;
	Cell* _good;
	int _id;
	Widget* _view;
};

class ShopPanel : public SimplePanel
{	
public:
	enum buttons
	{
	};
	ShopPanel(std::string name):SimplePanel(name){}
	static ShopPanel* create(std::string name);
	bool init();
private:
	Vector<GoodUnit*> _goodsUnit;
	ListView* _listView;
	Label* _label;
	Label* _title;
	Layout* _goodPan;
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void selectedItemEvent(Ref* pSender, ListView::EventType type);
	void selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type);
};


#endif /*__SHOP_PANEL_H__*/