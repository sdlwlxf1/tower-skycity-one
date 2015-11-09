#ifndef __BOOK_PANEL_H__
#define __BOOK_PANEL_H__

#include "UI/SimplePanel.h"
#include "cell/Cell.h"
#include "cell/actor/Actor.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class BookLine : public Ref
{
public:
	static BookLine* create(Actor* actor, Layout* layout);
	BookLine(Actor* actor, Layout* layout);
	bool init();
};

class BookPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1
	};
	BookPanel(std::string name):SimplePanel(name){}
	static BookPanel* create(std::string name);
	bool init();
	void initWithActors(Map<int, Cell*> map);
	CC_SYNTHESIZE(float, _gapV, GapV);
private:
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
private:
	Vector<BookLine*> _bookLines;
	Layout* _panelLine;
	Layout* _panelLines;
};


#endif /*__BOOK_PANEL_H__*/