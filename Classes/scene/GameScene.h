#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "map/World.h"
#include "ui/UILayout.h"
#include "UI/HudPanel.h"

USING_NS_CC;
using namespace ui;

class GameScene:public Scene
{
public:
	CREATE_FUNC(GameScene);
	virtual bool init();
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
	~GameScene();
	void plotUpdate(float dt);
	void resetInstance();
	HudPanel* getHud(){return _hud;}
private:
	void onHudTrigger(EventCustom *event);
	bool onTrigger(EventCustom *event);
	void onTouchDown(Ref *pSender, Widget::TouchEventType type);
	void bubbleInfo(std::string info, Vec2 site, Color3B color = Color3B::BLACK);
private:
	HudPanel *_hud;
	EventListenerCustom* _listener;
	World* _world;
	LayerColor *_color;
};


#endif /*__GAME_SCENE_H__*/
