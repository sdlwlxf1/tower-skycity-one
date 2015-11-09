#include "SimplePanel.h"
#include "StartPanel.h"
#include "ConfigPanel.h"
#include "PausePanel.h"
#include "ConveyPanel.h"
#include "PopPanel.h"
#include "ItemBagPanel.h"
#include "EndPanel.h"
#include "BookPanel.h"
#include "HudPanel.h"
#include "JoyStickPanel.h"
#include "LevelUpPanel.h"
#include "PaperPanel.h"
#include "ShopPanel.h"
#include "AchievementPanel.h"
#include "EndStatPanel.h"

#include "utils/StringHandler.h"
#include "model/Model.h"

#define NONE_IMAGE "none.png"

SimplePanel* SimplePanel::create(std::string name)
{
	SimplePanel *pRet = nullptr;
	if(name == "start")
	{
		pRet = new StartPanel(name);
	}
	else if(name == "config")
	{
		pRet = new ConfigPanel(name);
	}
	else if(name == "pause")
	{
		pRet = new PausePanel(name);
	}
	else if(name == "convey")
	{
		pRet = new ConveyPanel(name);
	}
	else if(name == "itemBag")
	{
		pRet = new ItemBagPanel(name);
	}
	else if(name == "end")
	{
		pRet = new EndPanel(name);
	}
	else if(name == "shop")
	{
		pRet = new ShopPanel(name);
	}
	else if(name == "book")
	{
		pRet = new BookPanel(name);
	}
	else if(name == "hud")
	{
		pRet = new HudPanel(name);
	}
	else if(name == "levelUp")
	{
		pRet = new LevelUpPanel(name);
	}
	else if(name == "paper")
	{
		pRet = new PaperPanel(name);
	}
	else if(name == "joyStick")
	{
		pRet = new JoyStickPanel(name);
	}
	else
	{
		pRet = new SimplePanel(name);
	}
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

SimplePanel::SimplePanel(std::string name)
{
	_url = StringUtils::format("UI/Export/UI_1/UI/%s.ExportJson",name.c_str());
	_root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(_url.c_str()));
	
	addChild(_root);
	_root->setTouchEnabled(false);
	_container = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_container"));
	_buttons = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_buttons"));

	setName(name);
}

bool SimplePanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(SimplePanel::onButtonClicked, this));
	}
	EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:menuOpen");
	return true;
}

void SimplePanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		CC_SAFE_RETAIN(this);
		this->removeFromParentAndCleanup(true);
	}
}

void SimplePanel::destroy()
{
	if(this->getParent()->getParent() == PopPanel::getInstance())
	{
		PopPanel::getInstance()->removePanel(this);
	}
	else
	{
		CC_SAFE_RETAIN(this);
		this->removeFromParentAndCleanup(this);
	}
}

void SimplePanel::onEnter()
{
	Layer::onEnter();
	_root->setOpacity(0);
	_root->runAction(FadeIn::create(0.1));
}

void SimplePanel::onExit()
{
	Layer::onExit();
}
