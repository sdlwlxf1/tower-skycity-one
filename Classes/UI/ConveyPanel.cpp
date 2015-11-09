#include "ConveyPanel.h"
#include "PopPanel.h"
#include "cell/actor/Actor.h"
#include "Detect.h"

#define NONE_IMAGE "none.png"
USING_NS_CC;

ConveyPanel* ConveyPanel::create(std::string name)
{
	ConveyPanel *pRet = new ConveyPanel(name);
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

bool ConveyPanel::init()
{
	_buttons = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_buttons"));
	_closeButton = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_close"));
	_closeButton->addTouchEventListener(CC_CALLBACK_2(ConveyPanel::onClose, this));
	_panelButton = static_cast<Layout*>(Helper::seekWidgetByName(_buttons, "Panel_button"));
	_panelButton->setPosition(Vec2(0,0));

	initWithUnlockMap(Detect::shareDetect()->getCurTower()->getUnlockFloorId());
	return true;
}

void ConveyPanel::initWithUnlockMap(std::map<int, bool> map)
{
	int i = 0;
	int colNum = 4;
	int rowNum = 4;
	float colGap = 20;
	float rowGap = 20;
	for(auto pair:map)
	{
		Widget* panelButton = _panelButton;
		auto id = pair.first;
		auto bol = pair.second;
		if(i >= 1)
		{
			panelButton = _panelButton->clone();
			_buttons->addChild(panelButton);
		}
		auto site = Vec2(
			(int)(_panelButton->getSize().width + colGap) * (i % 4),
			-(int)(_panelButton->getSize().height + rowGap) * (i / 4)
			);
		panelButton->setPosition(site);
		auto button = static_cast<Button*>(Helper::seekWidgetByName(panelButton, "Button_id"));
		auto text = static_cast<TextAtlas*>(Helper::seekWidgetByName(panelButton, "AtlasLabel_id"));
		text->setString(cocos2d::Value(id).asString());
		if(bol == false)
		{
			button->setBright(false);
			button->setEnabled(false);
		}
		else
		{
			button->setBright(true);
			button->setEnabled(true);
		}
		button->addTouchEventListener(CC_CALLBACK_2(ConveyPanel::onButtonClicked, this));
		button->setTag(id);
		i++;
	}
}

void ConveyPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		int id = button->getTag();

		auto detect = Detect::shareDetect();

		Player* player = detect->getPlayer();
		int curFloorId = detect->getCurFloor()->getId();
		if(curFloorId == id) return;
		int conveyCellId = curFloorId >= id ? 2005007 : 2005008;
		player->setConveyCount(2);
		EventHelper::getInstance()->dispatchByStr("target:"+player->getName()+"|type:convey","floorId:"+cocos2d::Value(id).asString()+"|toCellTypeId:"+cocos2d::Value(conveyCellId).asString());
		destroy();
	}
}

void ConveyPanel::onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		destroy();
	}
}

void ConveyPanel::destroy()
{
	PopPanel::getInstance()->removePanel(this);
}
