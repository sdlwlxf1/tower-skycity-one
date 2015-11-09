#include "scene/PreScene.h"
#include "UI/PrePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

bool PreScene::init()
{
	bool bRet = false;
	do{
		setName("preScene");
		_prePanel = PrePanel::create("pre");
		addChild(_prePanel);
		bRet = true;
	} while (0);
	return bRet;
}