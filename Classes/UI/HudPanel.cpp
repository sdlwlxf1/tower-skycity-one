#include "HudPanel.h"
#include "mutual/ChatManager.h"
#include "Detect.h"
#include "UI/BuffUnit.h"
#include "UI/BuffPanel.h"


#define NONE_IMAGE "none.png"

HudPanel* HudPanel::create(std::string name)
{
	HudPanel *pRet = new HudPanel(name);
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

HudPanel::~HudPanel()
{
	//_eventDispatcher->removeEventListener(_listener);
}

void HudPanel::onEnter()
{
	Layer::onEnter();

}

void HudPanel::onExit()
{
	Layer::onExit();

}

bool HudPanel::init()
{
	_joyStickLayOut = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_joyStickLayOut"));
	_joyStickPanel = JoyStickPanel::create("joyStick");
	_joyStickPanel->setPosition(Vec2(400,72));
	addChild(_joyStickPanel);

	_joyStickPanel->setDieRadius(100);//设置死亡半径（外圈）
	_joyStickPanel->setFailRadius(10);//设置失效半径（內圈）
	//_stick->setAutoPosition(true);
	Detect::shareDetect()->setJoyStick(_joyStickPanel);

	_popPanel = PopPanel::getInstance();

	_bubbleLabel = BubbleLabel::create();
	addChild(_bubbleLabel);

	_layerColor = LayerColor::create(Color4B(0,0,0,255));
	_layerColor->setContentSize(Director::getInstance()->getWinSize());
	_layerColor->setOpacity(0);
	addChild(_layerColor);

	_popPanel = PopPanel::getInstance();
	_popPanel->reset();
	addChild(_popPanel);
	ChatManager::getInstance()->setChatPanel(_popPanel->chatPanel);

	//_listener = EventListenerCustom::create("hud", CC_CALLBACK_1(HudPanel::onTrigger, this));
	_listener = EventListenerCustom::create("hud", [=](EventCustom *event){onTrigger(event);});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);

	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(HudPanel::onButtonClicked, this));
	}

	auto action1 = ScaleBy::create(0.15,1.5,1.5,1.0);
	_stress1 = Sequence::create(action1,action1->reverse(),nullptr);
	_stress1->retain();

	_plotBlock = false;

	_buffList = static_cast<ListView*>(Helper::seekWidgetByName(_root, "ListView_buffs"));
	updateBuffList();
	_buffList->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(HudPanel::selectedBuffEvent, this));
	return true;
}

void HudPanel::updateBuffList()
{
	auto buffUnits = _buffList->getItems();
	auto buffs = Detect::shareDetect()->getUser()->getBuffs();

	auto delBuffUnits = Vector<BuffUnit*>();
	for(auto pair:buffUnits)
	{
		auto buffUnit = dynamic_cast<BuffUnit*>(pair);
		if(buffUnit != nullptr)
		{
			auto exist = false;
			for(auto buff:buffs)
			{
				if(buffUnit->getBuff() == buff)
				{
					exist = true;
					break;
				}
			}
			if(exist == false)
			{
				delBuffUnits.pushBack(buffUnit);
			}
		}
	}

	for(auto pair:delBuffUnits)
	{
		_buffList->removeItem(_buffList->getIndex(pair));
	}
	delBuffUnits.clear();



	for(auto buff:buffs)
	{
		auto exist = false;
		for(auto pair:buffUnits)
		{
			auto buffUnit = dynamic_cast<BuffUnit*>(pair);
			if(buffUnit != nullptr && buffUnit->getBuff() == buff)
			{
				exist = true;
				break;
			}
		}
		if(exist == false)
		{
			auto buffUnit = BuffUnit::create();
			buffUnit->setBuff(buff);
			_buffList->pushBackCustomItem(buffUnit);
		}
	}
}

void HudPanel::selectedBuffEvent(Ref *pSender, ListView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
			//auto tag = listView->getItem(listView->getCurSelectedIndex())->getTag();
			//auto endStatUnit = _endStatsUnit.at(listView->getCurSelectedIndex());
			break;
		}
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child end index = %ld", listView->getCurSelectedIndex());
			auto buffUnit = static_cast<BuffUnit*>(listView->getItem(listView->getCurSelectedIndex()));
			
			auto buffPan = BuffPanel::create("buff");
			buffPan->initWithBuff(buffUnit->getBuff());
			PopPanel::getInstance()->addPanel(buffPan, 2);
			break;
		}
	default:
		break;
	}
}

void HudPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if(_plotBlock == true)
		return;
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case SHOP:
			{
				if(_popPanel->hasPanelByName("shop") != nullptr)
				{
					_popPanel->removePanelByName("shop");
				}
				else
				{
					_popPanel->removeAllPanelsByOrder(1);
					_popPanel->addPanelByName("shop",1);
				}
			}			
			break;
		case BAG:
			{
				if(_popPanel->hasPanelByName("itemBag") != nullptr)
				{
					_popPanel->removePanelByName("itemBag");
				}
				else
				{
					_popPanel->removeAllPanelsByOrder(1);
					_popPanel->addPanelByName("itemBag",1);
				}
			}
			break;
		case TRANS:
			{
				if(_popPanel->hasPanelByName("convey") != nullptr)
				{
					_popPanel->removePanelByName("convey");
				}
				else
				{
					_popPanel->removeAllPanelsByOrder(1);
					auto player = Detect::shareDetect()->getPlayer();
					auto device = player->getItemBag()->getSpecialByType(2007003);
					if(device == nullptr)
					{
						PopPanel::getInstance()->note("conveyNote","type:Text---text:"+ a2u("没有传送装置"));
						return;
					}
					if(conditions["conveyClose"].intValue() == 1)
					{
						PopPanel::getInstance()->note("conveyNote","type:Text---text:"+ a2u("魔法空间紊乱，无法传送"));
						return;
					}
					if(Detect::shareDetect()->getCurTower()->getCanConvey() == false)
					{
						PopPanel::getInstance()->note("conveyNote","type:Text---text:"+ a2u("此处无法传送"));
						return;
					}
					auto floor = Detect::shareDetect()->getCurFloor();
					int typeIds[] = {2005007,2005008};
					bool nearConvey = floor->getNearByTypeId(player->getSite(),SquareVecs,9,typeIds,2) != nullptr ? true:false;
					// convey debug
					nearConvey = true;

					if(nearConvey == true)
					{
						_popPanel->addPanelByName("convey",1);
					}
					else
					{
						PopPanel::getInstance()->note("conveyNote","type:Text---text:"+ a2u("只能在楼梯口附近传送"),1.2f,false);
					}
				}
			}
			break;
		case BOOK:
			{
				if(_popPanel->hasPanelByName("book") != nullptr)
				{
					_popPanel->removePanelByName("book");
				}
				else
				{
					_popPanel->removeAllPanelsByOrder(1);

					auto player = Detect::shareDetect()->getPlayer();
					auto device = player->getItemBag()->getSpecialByType(2007004);
					if(device == nullptr)
					{
						PopPanel::getInstance()->note("conveyNote","type:Text---text:"+ a2u("没有怪物手册，无法查看"),1.2f,false);
						return;
					}
					_popPanel->addPanelByName("book",1);
				}
			}
			break;
		case PAUSE:
			{
				if(_popPanel->hasPanelByName("pause") != nullptr)
				{
					_popPanel->removePanelByName("pause");
				}
				else
				{
					_popPanel->removeAllPanelsByOrder(1);
					_popPanel->addPanelByName("pause",1);
				}
			}
			break;
		}
		//CC_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}


void HudPanel::initModel()
{

}

void HudPanel::loadModel(ValueMap &map)
{

}

ValueMap HudPanel::saveModel()
{
	return ValueMap();
}

bool HudPanel::onTrigger(EventCustom *event)
{
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	auto senderName = (*data)["senderName"].asString();
	auto type = (*data)["type"].asString();

	if(type == "curtain")
	{
		auto action = effect["action"].asString();
		auto delay = effect["delay"].asFloat();
		if(action == "fadeIn")
		{
			_layerColor->runAction(FadeIn::create(delay));
		}
		else if(action == "fadeOut")
		{
			_layerColor->runAction(FadeOut::create(delay));
		}
	}
	else if(type == "plotBlock")
	{
		auto action = effect["action"].asString();
		if(action == "in")
		{
			_plotBlock = true;
		}
		else if(action == "out")
		{
			_plotBlock = false;
		}
	}
	auto player = Detect::shareDetect()->getPlayer();
	if(player != nullptr && senderName == player->getName())
	{
		if(type == "setHp")
		{
			auto hp = effect["num"].asInt();
			/*auto maxHp = effect["maxHp"].asInt();
			auto loadingBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(_root, "ProgressBar_hp"));
			loadingBar->setPercent(hp * 1.00 / maxHp * 100);*/
			auto text = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_hp"));
			text->setString(StringUtils::format("%d",hp));
		}
		else if(type == "setXp")
		{
			auto xp = effect["num"].asInt();
			auto lastXp = effect["lastXp"].asInt();
			auto nextXp = effect["nextXp"].asInt();
			auto loadingBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(_root, "ProgressBar_xp"));
			auto cur = xp - lastXp;
			auto max = nextXp - lastXp;
			//loadingBar->setPercent(cur * 1.00 / max * 100);
			auto text = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_xp"));
			text->setString(StringUtils::format("%d/%d",xp,nextXp));
		}
		else if(type == "setStr")
		{
			auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_str_num"));
			widget->setString(effect["num"].asString());
		}
		else if(type == "setDef")
		{
			auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_def_num"));
			widget->setString(effect["num"].asString());
		}
		else if(type == "setLevel")
		{
			auto widget = static_cast<TextAtlas*>(Helper::seekWidgetByName(_root, "Label_level_num"));
			widget->setString(effect["num"].asString());
		}
		else if(type == "setGold")
		{
			auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_gold_num"));
			widget->setString(effect["num"].asString());
		}

		else if(type == "addHp")
		{
			std::string str = "{0,0}";
			if(!effect["site"].isNull()) str = effect["site"].asString();
			Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(str));
			std::string add = (effect["num"].asInt()>0?"+":"") + effect["num"].asString();
			_bubbleLabel->addLabel(GETSTRING("hp") + add,site);

			if(effect["num"].asInt() > 0)
			{
				auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_hp"));
				widget->setScale(1.0f);
				//widget->runAction(_stress1->clone());
			}
		}
		else if(type == "addXp")
		{
			std::string str = "{0,0}";
			if(!effect["site"].isNull()) str = effect["site"].asString();
			Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(str));
			std::string add = (effect["num"].asInt()>0?"+":"") + effect["num"].asString();
			_bubbleLabel->addLabel(GETSTRING("xp") + add,site);

			if(effect["num"].asInt() > 0)
			{
				auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_xp"));
				widget->setScale(1.0f);
				//widget->runAction(_stress1->clone());
			}
		}
		else if(type == "addStr")
		{
			std::string str = "{0,0}";
			if(!effect["site"].isNull()) str = effect["site"].asString();
			Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(str));
			std::string add = (effect["num"].asInt()>0?"+":"") + effect["num"].asString();
			_bubbleLabel->addLabel(GETSTRING("str") + add,site);

			if(effect["num"].asInt() > 0)
			{
				auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_str_num"));
				widget->setScale(1.0f);
				//widget->runAction(_stress1->clone());
			}
		}
		else if(type == "addDef")
		{
			std::string def = "{0,0}";
			if(!effect["site"].isNull()) def = effect["site"].asString();
			Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(def));
			std::string add = (effect["num"].asInt()>0?"+":"") + effect["num"].asString();
			_bubbleLabel->addLabel(GETSTRING("def") + add,site);

			if(effect["num"].asInt() > 0)
			{
				auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_def_num"));
				widget->setScale(1.0f);
				//widget->runAction(_stress1->clone());
			}
		}
		else if(type == "addLevel")
		{
			std::string level = "{0,0}";
			if(!effect["site"].isNull()) level = effect["site"].asString();
			Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(level));
			std::string add = (effect["num"].asInt()>0?"+":"") + effect["num"].asString();
			_bubbleLabel->addLabel(GETSTRING("level") + add,site);

			if(effect["num"].asInt() > 0)
			{
				auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_level_num"));
				widget->setScale(1.0f);
				//widget->runAction(_stress1->clone());
			}
		}
		else if(type == "addGold")
		{
			std::string gold = "{0,0}";
			if(!effect["site"].isNull()) gold = effect["site"].asString();
			Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(gold));
			std::string add = (effect["num"].asInt()>0?"+":"") + effect["num"].asString();
			_bubbleLabel->addLabel(GETSTRING("gold") + add,site);

			if(effect["num"].asInt() > 0)
			{
				auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_gold_num"));
				widget->setScale(1.0f);
				//widget->runAction(_stress1->clone());
			}
		}
		else if(type == "otherNote")
		{
			std::string str = "{0,0}";
			if(!effect["site"].isNull()) str = effect["site"].asString();
			Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(str));
			std::string text = effect["text"].asString();
			_bubbleLabel->addLabel(text,site);
		}
		else if(type == "addProp")
		{
			auto propType = effect["propType"].asInt();
			auto addPropNum = effect["addPropNum"].asInt();
			auto propNum = effect["propNum"].asInt();

			if(addPropNum != 0)
			{
				switch (propType)
				{
				case 2002002:case 2002001:case 2002003:
					std::string str = "{0,0}";
					if(!effect["site"].isNull()) str = effect["site"].asString();
					Vec2 site = Detect::shareDetect()->getWorld()->convertToWorldSpace(PointFromString(str));
					std::string add = GETSTRING("get")+ effect["propName"].asString() + (effect["addPropNum"].asInt()>0?"+":"") + effect["addPropNum"].asString();
					_bubbleLabel->addLabel(add,site);
					break;
				}
			}


			switch (propType)
			{
			case 2002002:
				{
					auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_key_1_num"));
					widget->setString(effect["propNum"].asString());
					widget->setScaleX(1.0f);
					widget->setScaleY(1.0f);
					//widget->runAction(_stress1->clone());

				}
				break;
			case 2002001:
				{
					auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_key_2_num"));
					widget->setString(effect["propNum"].asString());
					widget->setScaleX(1.0f);
					widget->setScaleY(1.0f);
					//widget->runAction(_stress1->clone());
				}
				break;
			case 2002003:
				{
					auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_key_3_num"));
					widget->setString(effect["propNum"].asString());
					widget->setScaleX(1.0f);
					widget->setScaleY(1.0f);
					//widget->runAction(_stress1->clone());
				}
				break;
			default:
				break;
			}
		}
		else if(type == "removeProp")
		{
			auto propType = effect["propType"].asInt();
			auto addPropNum = effect["addPropNum"].asInt();
			auto propNum = effect["propNum"].asInt();

			switch (propType)
			{
			case 2002002:
				{
					auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_key_1_num"));
					widget->setString(effect["propNum"].asString());
				}
				break;
			case 2002001:
				{
					auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_key_2_num"));
					widget->setString(effect["propNum"].asString());
				}
				break;
			case 2002003:
				{
					auto widget = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_key_3_num"));
					widget->setString(effect["propNum"].asString());
				}
				break;
			default:
				break;
			}
		}

	}

	if(type == "setCurFloor")
	{
		auto widget1 = static_cast<TextAtlas*>(Helper::seekWidgetByName(_root, "Label_floor_num"));
		widget1->setString(effect["floorId"].asString());
		auto widget2 = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_tower_name"));
		widget2->setString(effect["towerName"].asString());
	}
	return false;
}

const std::string HudPanel::getNameId()
{
	return this->getName();
}

std::string HudPanel::getType()
{
	return "hudPanel";
}