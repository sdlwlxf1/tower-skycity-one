#include "scene/StartScene.h"
#include "UI/StartPanel.h"
#include "utils/EventHelper.h"
#include "Detect.h"
#include "ui/PopPanel.h"
#include "ui/ShareAwardPanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

bool StartScene::init()
{
	bool bRet = false;
	do{
		setName("startScene");
		_startPanel = StartPanel::create("start");
		addChild(_startPanel);

		addChild(PopPanel::getInstance());
		
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playBG",StringUtils::format("name:titleBg|loop:1"));

		bRet = true;
	} while (0);
	return bRet;
}
