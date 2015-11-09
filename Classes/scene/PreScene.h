#ifndef __PRE_SCENE_H__
#define __PRE_SCENE_H__

#include "cocos2d.h"
#include "UI/PrePanel.h"

USING_NS_CC;
using namespace ui;

class PreScene:public Scene
{
public:
	CREATE_FUNC(PreScene);
	virtual bool init();
	/*virtual void onEnter();
	virtual void onExit();
	~PreScene();*/
private:
	PrePanel* _prePanel;
};

#endif /*__PRE_SCENE_H__*/