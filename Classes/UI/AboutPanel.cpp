#include "AboutPanel.h"

#define NONE_IMAGE "none.png"

AboutPanel* AboutPanel::create(std::string name)
{
	AboutPanel *pRet = new AboutPanel(name);
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

bool AboutPanel::init()
{
	/*for(auto node:_buttons->getChildren())
	{
	auto button = static_cast<Button*>(node);
	button->addTouchEventListener(CC_CALLBACK_2(AboutPanel::onButtonClicked, this));
	}*/
	
	_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	_root->addTouchEventListener(CC_CALLBACK_2(AboutPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

void AboutPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		destroy();
	}
}

void AboutPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}