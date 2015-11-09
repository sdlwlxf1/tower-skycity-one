#include "MMPanel.h"
#include "scene/StartScene.h"
#include "ui/LogoPanel.h"

#define NONE_IMAGE "none.png"

MMPanel* MMPanel::create(std::string name)
{
	MMPanel *pRet = new MMPanel(name);
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

bool MMPanel::init()
{
	/*for(auto node:_buttons->getChildren())
	{
	auto button = static_cast<Button*>(node);
	button->addTouchEventListener(CC_CALLBACK_2(MMPanel::onButtonClicked, this));
	}*/
	
	/*_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));

	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));*/

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	_root->addTouchEventListener(CC_CALLBACK_2(MMPanel::onButtonClicked, this));
	setKeyboardEnabled(true);

	_logo = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_logo"));

	_logo->setOpacity(0);
	_logo->runAction(Sequence::create(FadeIn::create(3),DelayTime::create(2),CallFunc::create([=](){end();}),nullptr));

	return true;
}

void MMPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		_logo->stopAllActions();
		_logo->setOpacity(255);
		end();
	}
}

void MMPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		end();
	}
}

void MMPanel::end()
{
	/*auto s = StartScene::create();
	auto scene = TransitionFade::create(3.0f, s, Color3B(255,255,255));
	if (scene)
	{
		Director::getInstance()->replaceScene(scene);
	}*/
	auto logoPanel = LogoPanel::create("logo");
	this->getScene()->addChild(logoPanel);
}