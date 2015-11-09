#ifndef __FIGHT_PANEL_H__
#define __FIGHT_PANEL_H__

#include "cocos2d.h"
#include "cell/actor/Actor.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class FightPanel : public Layer
{	
public:
	FightPanel(Actor* player, Actor* rival);
	static FightPanel* create(Actor* player, Actor* rival);
private:
	Layout* _fightPan;
	Actor* _player;
	Actor* _rival;
	ImageView* _playerImg;
	ImageView* _rivalImg;
	LoadingBar* _playerBar;
	LoadingBar* _rivalBar;
	Button* _closeBtn;
private:
	void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onUse(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__FIGHT_PANEL_H__*/