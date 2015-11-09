#include "StartPanel.h"
#include "scene/GameScene.h"
#include "UI/FilePanel.h"
#include "UI/ConfigPanel.h"
#include "UI/AchievementPanel.h"
#include "Detect.h"
#include "ui/AboutPanel.h"

#define NONE_IMAGE "none.png"

StartPanel* StartPanel::create(std::string name)
{
	StartPanel *pRet = new StartPanel(name);
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

bool StartPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(StartPanel::onButtonClicked, this));
	}
	setKeypadEnabled(true);
	setKeyboardEnabled(true);

	/*ParticleSystemQuad* smokeEmitter = ParticleSystemQuad::create("particles/cloud.plist");
	smokeEmitter->retain();
	smokeEmitter->setName("cloud");
	addChild(smokeEmitter ,1);*/

	
	auto cloudLayout = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_cloud"));

	auto cloud1 = Sprite::createWithSpriteFrameName("cloud1.png");
	auto width = cloud1->getContentSize().width;
	cloud1->runAction(RepeatForever::create(Sequence::create(MoveTo::create(20,Vec2(width + 400,0)),DelayTime::create(2),CallFunc::create([=](){cloud1->setPosition(0,0);}),nullptr)));
	cloudLayout->addChild(cloud1);

	auto cloud2 = Sprite::createWithSpriteFrameName("cloud2.png");
	cloud2->setPosition(-width - 10,120);
	cloud2->runAction(RepeatForever::create(Sequence::create(MoveTo::create(30,Vec2(width + 400,120)),DelayTime::create(2),CallFunc::create([=](){cloud2->setPosition(-width-10,120);}),nullptr)));
	cloudLayout->addChild(cloud2);

	auto cloud3 = Sprite::createWithSpriteFrameName("cloud2.png");
	cloud3->setPosition(-width + 10,80);
	cloud3->runAction(RepeatForever::create(Sequence::create(MoveTo::create(25,Vec2(width + 400,80)),DelayTime::create(2),CallFunc::create([=](){cloud3->setPosition(-width + 10,80);}),nullptr)));
	cloudLayout->addChild(cloud3);

	return true;
}

void StartPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case BEGIN:
			{
				if(Detect::shareDetect()->getUser()->getFirstOpen() == true)
				{
					Detect::shareDetect()->load = false;
					auto s = GameScene::create();
					auto scene = TransitionFade::create(1.0f, s, Color3B::BLACK);
					if (scene)
					{
						Director::getInstance()->replaceScene(scene);
					}
				}
				else
				{
					auto file = FilePanel::create("file");
					file->setType(FilePanel::LOAD);
					PopPanel::getInstance()->addPanel(file,1);
				}
			}			
			break;
		case OPTION:
			{
				auto config = ConfigPanel::create("config");
				PopPanel::getInstance()->addPanel(config,1);
			}
			break;
		case ACHIEVE:
			{
				auto achieve = AchievementPanel::create("achievement");
				achieve->initWithModel(Detect::shareDetect()->getUser()->getAchieves());
				PopPanel::getInstance()->addPanel(achieve,1);
			}
			break;
		case ABOUT:
			{
				auto about = AboutPanel::create("about");
				PopPanel::getInstance()->addPanel(about,1);
			}
			break;
		}
		//_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}

void StartPanel::onEnter()
{
	SimplePanel::onEnter();
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->setOpacity(0);
	}
	auto image = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_mota"));
	image->setOpacity(0);
	image->runAction(FadeIn::create(1.0));
	image->runAction(Sequence::create(MoveTo::create(1.0,Vec2(image->getPosition().x,image->getPosition().y + 10)),CallFunc::create([this](){
		for(auto node:_buttons->getChildren())
		{
			auto button = static_cast<Button*>(node);
			button->runAction(FadeIn::create(1.0));
		}
	}),NULL));
	
}

void StartPanel::onExit()
{
	SimplePanel::onExit();
}

void StartPanel::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
	{
		if(this->getChildByName("exitConfirm") != nullptr)
			return;
		auto pan = PromptPanel::create("prompt");
		pan->initWithContent("type:Text---text:"+ a2u("确定要退出游戏吗？"),PromptPanel::TYPE_DONE_CANCEL);
		pan->onDone = [=](){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
			MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
			return;
#endif

			Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			exit(0);
#endif
		};
		pan->setCenter();
		pan->setName("exitConfirm");
		addChild(pan);
		log("back button clicked");
	}
	else if(keyCode == EventKeyboard::KeyCode::KEY_MENU)
	{
		log("menu button clicked");
	}
	else if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE)
	{
		log("backspace button clicked");
	}
}