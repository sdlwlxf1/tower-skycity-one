#include "SharePanel.h"
#include "scene/GameScene.h"
#include "UI/FilePanel.h"
#include "UI/ConfigPanel.h"
#include "UI/AchievementPanel.h"
#include "Detect.h"
#include "ui/AboutPanel.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform\android\jni\JniHelper.h"
#else
//#include "JniTest.h"
#endif

#define NONE_IMAGE "none.png"

SharePanel* SharePanel::create(std::string name)
{
	SharePanel *pRet = new SharePanel(name);
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

void SharePanel::setInfo(std::string info)
{
	_info = info;
}

bool SharePanel::init()
{
	_info = "";

	_shareWaysPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_shareWays"));

	for(auto node:_shareWaysPan->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(SharePanel::onShare, this));
	}

	/*auto shareBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_share"));
	shareBtn->addTouchEventListener(CC_CALLBACK_2(SharePanel::onShare, this));*/

	/*auto authBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_auth"));
	authBtn->addTouchEventListener(CC_CALLBACK_2(SharePanel::onAuth, this));*/

	auto closeBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_close"));
	closeBtn->addTouchEventListener(CC_CALLBACK_2(SharePanel::onClose, this));

	_root->setTouchEnabled(true);
	return true;
}

void SharePanel::onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		destroy();
	}
}

// ����
void SharePanel::onShare(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		Button *button = (Button*)ref;
		auto tag = button->getTag();
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		//ȷ������
		JniMethodInfo jmi;
		if(JniHelper::getStaticMethodInfo(jmi ,"IAP/IAPJni" ,"shareOrder" ,"(ILjava/lang/String;)V"))
		{
			jstring jstr = jmi.env->NewStringUTF(_info.c_str());
			jmi.env->CallStaticVoidMethod(jmi.classID , jmi.methodID, tag, jstr);
		}
#else
#endif
	}
}

void SharePanel::onEnter()
{
	SimplePanel::onEnter();
}

void SharePanel::onExit()
{
	SimplePanel::onExit();
}
