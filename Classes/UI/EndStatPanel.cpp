#include "EndStatPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"

#define NONE_IMAGE "none.png"

EndStatPanel* EndStatPanel::create(std::string name)
{
	EndStatPanel *pRet = new EndStatPanel(name);
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

bool EndStatPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(EndStatPanel::onButtonClicked, this));
	}

	_labelHp = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_hp"));
	_labelStr = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_str"));
	_labelDef = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_def"));
	_labelGold = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_gold"));
	_labelXp = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_xp"));
	_labelHoeNum = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_hoe_num"));
	_labelBombNum = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_bomb_num"));
	_labelTime = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_time"));
	_labelEndTag = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_end_tag"));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	//_root->addTouchEventListener(CC_CALLBACK_2(EndStatPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

void EndStatPanel::initWithModel(ValueMap model)
{
	_labelHp->setString(model["hp"].asString());
	_labelStr->setString(model["str"].asString());
	_labelDef->setString(model["def"].asString());
	_labelGold->setString(model["gold"].asString());
	_labelXp->setString(model["xp"].asString());
	_labelHoeNum->setString(model["hoeNum"].asString());
	_labelBombNum->setString(model["bombNum"].asString());
	
	auto durTime = model["durTime"].asInt();
	int shi = durTime/3600;
	int fen = durTime/60%60;
	int miao = durTime%60;
	char date[32] = {0};
	sprintf(date, "%02d:%02d:%02d",shi,fen,miao);
	_labelTime->setString(StringUtils::format("%s",date));

	_labelEndTag->setString(model["endTag"].asString());
}

void EndStatPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			
			break;
		}
		destroy();
	}
}

void EndStatPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}
