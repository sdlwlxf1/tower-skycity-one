#ifndef __PRE_SCENE_H__
#define __PRE_SCENE_H__

#include "cocos2d.h"
#include "UI/MMPanel.h"

USING_NS_CC;
using namespace ui;

class LogoScene:public Scene
{
public:
	CREATE_FUNC(LogoScene);
	virtual bool init();
	/*virtual void onEnter();
	virtual void onExit();
	~LogoScene();*/
private:
	MMPanel* _mmPanel;
};

#endif /*__PRE_SCENE_H__*/
