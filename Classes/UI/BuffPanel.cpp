#include "BuffPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"

#define NONE_IMAGE "none.png"

BuffPanel* BuffPanel::create(std::string name)
{
	BuffPanel *pRet = new BuffPanel(name);
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

bool BuffPanel::init()
{
	_buff = nullptr;

	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(BuffPanel::onButtonClicked, this));
	}
	
	_container->setAnchorPoint(Vec2(0,0));
	_nameLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_name"));
	_introLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_intro"));
	_remTimeLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_remTime"));

	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	//_root->addTouchEventListener(CC_CALLBACK_2(BuffPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

void BuffPanel::initWithBuff(Buff* buff)
{
	_buff = buff;
	_buff->setBuffPanel(this);
	_nameLab->setString(buff->getModelByName("nickName").asString());
	_introLab->setString(buff->getModelByName("introduce").asString());
	// 剩余时间
	updateBuffTime(_buff->getRemTime());
}

// 剩余时间
void BuffPanel::updateBuffTime(double time)
{
	int durTime = time;
	int shi = durTime/3600;
	int fen = durTime/60%60;
	int miao = durTime%60;
	char date[32] = {0};
	sprintf(date, "%02d:%02d:%02d",shi,fen,miao);
	_remTimeLab->setString(StringUtils::format("%s",date));
}

void BuffPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
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

void BuffPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}

void BuffPanel::destroy()
{
	_buff->setBuffPanel(nullptr);
	setBuff(nullptr);
	SimplePanel::destroy();
}
