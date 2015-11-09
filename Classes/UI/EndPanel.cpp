#include "EndPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "scene/GameScene.h"
#include "Detect.h"
#include "utils/Tools.h"
#include "ui/EndStatPanel.h"

#define NONE_IMAGE "none.png"

EndPanel* EndPanel::create(std::string name)
{
	EndPanel *pRet = new EndPanel(name);
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

bool EndPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(EndPanel::onButtonClicked, this));
	}
	_statBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_stat"));
	return true;
}

void EndPanel::initWithNum(int id)
{
	std::string endTextName = "endText" + cocos2d::Value(id).asString();

	auto contentStr = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", endTextName.c_str(),"content");
	auto titleStr = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", endTextName.c_str(),"title");

	auto title = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_title"));
	auto content = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_content"));

	title->setString(titleStr);
	content->setString(contentStr);

	// 记录通关成绩
	_statBtn->setVisible(id > 1?true:false);
	if(id > 1)
	{
		auto player = Detect::shareDetect()->getPlayer();
		if(player == nullptr)
			return;
		ValueMap model;
		model["hp"] = player->getHp();
		model["str"] = player->getStr();
		model["def"] = player->getDef();
		model["xp"] = player->getXp();
		model["gold"] = player->getGold();
		model["bombNum"] = player->getUseBombNum();
		model["hoeNum"] = player->getUseHoeNum();
		model["durTime"] = player->getDurTime();
		model["curTime"] = getTimeStr();
		model["endTag"] = titleStr;
		Detect::shareDetect()->getUser()->pushAchieve(model);
	}

	auto black = LayerColor::create(Color4B::BLACK);
	black->setOpacity(0);
	_root->addChild(black);

	_container->retain();
	_container->removeFromParent();
	_container->setAnchorPoint(Vec2(0,0));
	
	black->runAction(Sequence::create(FadeIn::create(2.0f),DelayTime::create(1),CallFunc::create([=](){
		this->addChild(_container);
		_container->release();
		_container->setScale(0.85f);
		_container->setOpacity(200);
		_container->runAction(FadeIn::create(0.4));
		_container->runAction(ScaleTo::create(0.4,1.0f,1.0f,1.0f));
	}),nullptr));
}

void EndPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case BACK:
			{
				destroy();
				auto gameScene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
				gameScene->resetInstance();
				auto scene = StartScene::create();
				Director::getInstance()->replaceScene(scene);
			}			
			break;
		case STAT:
			{
				auto user = Detect::shareDetect()->getUser();
				auto endStatPan = EndStatPanel::create("endStat");
				endStatPan->initWithModel(user->getLastAchieve());

				PopPanel::getInstance()->addPanel(endStatPan, 2);
			}
			break;
		}

		//CC_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}