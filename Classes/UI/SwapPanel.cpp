#include "SwapPanel.h"
#include "scene/GameScene.h"
#include "UI/FilePanel.h"
#include "UI/ConfigPanel.h"


#define NONE_IMAGE "none.png"

SwapPanel* SwapPanel::create(std::string name)
{
	SwapPanel *pRet = new SwapPanel(name);
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

bool SwapPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(SwapPanel::onButtonClicked, this));
	}
	_title = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_title"));
	_content = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_content"));
	_introPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_intro"));
	_introText = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_intro_text"));
	auto buy = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_ok"));
	buy->addTouchEventListener(CC_CALLBACK_2(SwapPanel::onButtonClicked, this));
	return true;
}

void SwapPanel::build(Swap* swap)
{
	_swap = swap;
	_title->setString(swap->title);

	Vector<MenuItem *> arrayOfItems;

	auto options = swap->options;
	int i = 0;
	for(auto pair:options)
	{
		auto option = pair.asValueVector();
		auto buyerVec = option.at(0).asValueVector();
		auto sellerVec = option.at(1).asValueVector();
		std::string optionStr = "";
		for(auto pair:buyerVec.at(1).asValueMap())
		{
			auto type = pair.first;
			auto num = pair.second;
			if(type == "none")
			{
				optionStr += a2u("白给他");
			}
			else if(type == "gold")
			{
				optionStr += a2u("增加金币")+num.asString();
			}
			else if(type == "hp")
			{
				optionStr += a2u("增加生命力")+num.asString();
			}
			else if(type == "str")
			{
				optionStr += a2u("增加攻击力")+num.asString();
			}
			else if(type == "def")
			{
				optionStr += a2u("增加防御力")+num.asString();
			}
			else if(type == "xp")
			{
				optionStr += a2u("增加经验")+num.asString();
			}
			else if(type == "level")
			{
				optionStr += a2u("提高等级")+num.asString();
			}
			else if(type == "key1")
			{
				optionStr += a2u("获得蓝钥匙")+num.asString();
			}
			else if(type == "key2")
			{
				optionStr += a2u("获得黄钥匙")+num.asString();
			}
			else if(type == "key3")
			{
				optionStr += a2u("获得红钥匙")+num.asString();
			}
			else if(type == "sparPatch")
			{
				optionStr += a2u("获得灵魂石")+num.asString();
			}
			//KB,PJ,BJ,XX,LJ,RD,RH,GD,FT,SB
			// 狂暴、破甲、暴击、吸血、连击、肉盾、弱化、格挡、反弹、闪避
			else if(type == "KBspar")
			{
				optionStr += a2u("狂暴晶石");
			}
			else if(type == "PJspar")
			{
				optionStr += a2u("破甲晶石");
			}
			else if(type == "BJspar")
			{
				optionStr += a2u("暴击晶石");
			}
			else if(type == "XXspar")
			{
				optionStr += a2u("吸血晶石");
			}
			else if(type == "LJspar")
			{
				optionStr += a2u("连击晶石");
			}
			else if(type == "RDspar")
			{
				optionStr += a2u("肉盾晶石");
			}
			else if(type == "RHspar")
			{
				optionStr += a2u("弱化晶石");
			}
			else if(type == "GDspar")
			{
				optionStr += a2u("格挡晶石");
			}
			else if(type == "FTspar")
			{
				optionStr += a2u("反弹晶石");
			}
			else if(type == "SBspar")
			{
				optionStr += a2u("闪避晶石");
			}
		}
		//auto label = Label::createWithBMFont("UI/Export/UI_1/UI/common/font/font1_0.fnt", optionStr);
		auto label = Label::createWithSystemFont(optionStr,"Arial",20,Size::ZERO,TextHAlignment::CENTER);
		label->setColor(Color3B::WHITE);
		auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(SwapPanel::menuCallback, this));
		arrayOfItems.pushBack(menuItem);
		menuItem->setTag(i);
		i++;
	}
	_menu = Menu::createWithArray(arrayOfItems);
	_content->addChild(_menu);
	_menu->alignItemsVertically();
	setSelected(-1);
}

void SwapPanel::menuCallback(Ref* ref)
{
	auto sitem = static_cast<MenuItemLabel*>(ref);
	auto tag = sitem->getTag();
	setSelected(tag);
	
	//label->enableGlow(Color4B::RED);
	for(auto node:_menu->getChildren())
	{
		auto item = static_cast<MenuItemLabel*>(node);
		auto label = static_cast<Label*>(item->getLabel());
		label->setColor(Color3B::WHITE);
	}
	auto slabel = static_cast<Label*>(sitem->getLabel());
	slabel->setColor(Color3B::RED);
}

void SwapPanel::setSelected(int num)
{
	_selected = num;
	if(num == -1)
	{
		_introPan->setVisible(false);
		_introPan->setEnabled(false);
	}
	else
	{
		_introPan->setVisible(true);
		_introPan->setEnabled(true);
		auto option = _swap->options.at(num).asValueVector();
		auto buyerVec = option.at(0).asValueVector();
		auto sendMap = buyerVec.at(0).asValueMap();
		std::string sendStr = "";
		for(auto pair:sendMap)
		{
			auto type = pair.first;
			auto num = pair.second;
			if(type == "gold")
			{
				sendStr += a2u("消耗金币")+num.asString();
			}
			else if(type == "hp")
			{
				sendStr += a2u("消耗生命")+num.asString();
			}
			else if(type == "str")
			{
				sendStr += a2u("消耗攻击力")+num.asString();
			}
			else if(type == "def")
			{
				sendStr += a2u("消耗防御力")+num.asString();
			}
			else if(type == "xp")
			{
				sendStr += a2u("消耗经验")+num.asString();
			}
			else if(type == "level")
			{
				sendStr += a2u("消耗等级")+num.asString();
			}
			else if(type == "key1")
			{
				sendStr += a2u("消耗蓝钥匙")+num.asString();
			}
			else if(type == "key2")
			{
				sendStr += a2u("消耗黄钥匙")+num.asString();
			}
			else if(type == "key3")
			{
				sendStr += a2u("消耗红钥匙")+num.asString();
			}
			else if(type == "sparPatch")
			{
				sendStr += a2u("消耗晶石碎片")+num.asString();
			}
		}
		_introText->setString(sendStr);
	}
}

void SwapPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			{
				destroy();
			}			
			break;
		case BUY:
			{
				setSelected(_selected);
				_swap->deal(_selected);
			}
			break;
		}

		//_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}

void SwapPanel::onEnter()
{
	SimplePanel::onEnter();
}

void SwapPanel::onExit()
{
	SimplePanel::onExit();
}

void SwapPanel::destroy()
{
	_swap->removeFromParent();
	PopPanel::getInstance()->removePanel(this);
}