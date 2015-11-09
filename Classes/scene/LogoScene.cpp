#include "scene/LogoScene.h"

USING_NS_CC;

bool LogoScene::init()
{
	bool bRet = false;
	do{
		setName("logoScene");
		_mmPanel = MMPanel::create("mm");
		addChild(_mmPanel);
		bRet = true;
	} while (0);
	return bRet;
}
