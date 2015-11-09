#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"
#include "UI/StartPanel.h"

USING_NS_CC;
using namespace ui;

class StartScene:public Scene
{
public:
	CREATE_FUNC(StartScene);
	virtual bool init();
	/*virtual void onEnter();
	virtual void onExit();
	~StartScene();*/
private:
	StartPanel* _startPanel;
};

#endif /*__START_SCENE_H__*/