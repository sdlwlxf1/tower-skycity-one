#include "LevelUpPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"

#define NONE_IMAGE "none.png"

LevelUpPanel* LevelUpPanel::create(std::string name)
{
	LevelUpPanel *pRet = new LevelUpPanel(name);
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

bool LevelUpPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(LevelUpPanel::onButtonClicked, this));
	}
	
	_container->setAnchorPoint(Vec2(0,0));
	_labelAddHp = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_add_hp"));
	_labelAddStr = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_add_str"));
	_labelAddDef = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_add_def"));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	//_root->addTouchEventListener(CC_CALLBACK_2(LevelUpPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

void LevelUpPanel::initWithNums(int num[])
{
	_labelAddHp->setString("+" + cocos2d::Value(num[0]).asString());
	_labelAddStr->setString("+" + cocos2d::Value(num[1]).asString());
	_labelAddDef->setString("+" + cocos2d::Value(num[2]).asString());
}

void LevelUpPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
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

void LevelUpPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}
