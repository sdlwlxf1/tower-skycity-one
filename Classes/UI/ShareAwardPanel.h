#ifndef __SHARE_AWARD_PANEL_H__
#define __SHARE_AWARD_PANEL_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "SimplePanel.h"
#include "cocostudio/CCSGUIReader.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class ShareAwardPanel : public SimplePanel
{	
public:

	enum SELECT_WAY
	{
		Wechat,
		Qzone,
		Tencent,
		Sina
	};
	void onEnter();
	void onExit();
	ShareAwardPanel(std::string name):SimplePanel(name){}
	static ShareAwardPanel* create(std::string name);
	bool init();
	void updateIntro();
private:
	Layout* _shareWaysPan;
	int _selected;
private:
	void setSelectShareWay(int index);
	void onSelectShareWays(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onAuth(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onShare(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	
};


#endif /*__SHARE_AWARD_PANEL_H__*/