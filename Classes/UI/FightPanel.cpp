#include "FightPanel.h"
#include "utils/StringHandler.h"
#include "model/Model.h"

#define NONE_IMAGE "none.png"

FightPanel* FightPanel::create(Actor* player, Actor* rival)
{
	FightPanel *pRet = new FightPanel(player, rival);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

FightPanel::FightPanel(Actor* player, Actor* rival)
{
	_player = player;
	_rival = rival;
	_fightPan = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI/Export/UI_1/UI/fight.ExportJson"));

	_playerImg = static_cast<ImageView*>(Helper::seekWidgetByName(_fightPan, "Image_player"));
	_rivalImg = static_cast<ImageView*>(Helper::seekWidgetByName(_fightPan, "Image_rival"));
	_playerBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(_fightPan, "ProgressBar_player"));
	_rivalBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(_fightPan, "ProgressBar_rival"));
	_closeBtn = static_cast<Button*>(Helper::seekWidgetByName(_fightPan, "Button_close"));

	_closeBtn->addTouchEventListener(CC_CALLBACK_2(FightPanel::onClose, this));
	addChild(_fightPan);
}

void FightPanel::onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		CC_SAFE_RETAIN(this); 
		this->removeFromParentAndCleanup(true);
	}
}