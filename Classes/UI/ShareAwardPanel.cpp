#include "ShareAwardPanel.h"
#include "utils/StringHandler.h"
#include "model/Model.h"
#include "PopPanel.h"
#include "Detect.h"
#include "cell/CellFactory.h"
#include "utils/PlotScript.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform\android\jni\JniHelper.h"
#else
//#include "JniTest.h"
#endif

#define NONE_IMAGE "none.png"

ShareAwardPanel* ShareAwardPanel::create(std::string name)
{
	ShareAwardPanel *pRet = new ShareAwardPanel(name);
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

void ShareAwardPanel::onEnter()
{
	Layer::onEnter();
}

void ShareAwardPanel::onExit()
{
	Layer::onExit();
}

bool ShareAwardPanel::init()
{
	_shareWaysPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_shareWays"));

	for(auto node:_shareWaysPan->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(ShareAwardPanel::onSelectShareWays, this));
	}
	
	auto shareBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_share"));
	shareBtn->addTouchEventListener(CC_CALLBACK_2(ShareAwardPanel::onShare, this));

	/*auto authBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_auth"));
	authBtn->addTouchEventListener(CC_CALLBACK_2(ShareAwardPanel::onAuth, this));*/

	auto closeBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_close"));
	closeBtn->addTouchEventListener(CC_CALLBACK_2(ShareAwardPanel::onClose, this));

	auto tip = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_tip"));
	

	int defaultWay = 1;
	for(int i = 0; i < 3; i++)
	{
		auto user = Detect::shareDetect()->getUser();
		if(user->getShareRewardsLock(i) == 0)
		{
			defaultWay = i + 1;
		}
		
	}
	setSelectShareWay(defaultWay);

	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(EaseElasticOut::create(ScaleTo::create(1.0,1.0,1.0)));

	_root->setTouchEnabled(true);
	return true;
}

void ShareAwardPanel::onSelectShareWays(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) {
		Button *button = (Button*)ref;

		for(auto node:_shareWaysPan->getChildren())
		{
			auto btn = static_cast<Button*>(node);
			if(btn != button)
			{
				btn->runAction(EaseElasticOut::create(ScaleTo::create(1.0,1.0,1.0)));
			}
		}
		button->runAction(EaseElasticOut::create(ScaleTo::create(1.0,1.2,1.2)));

		setSelectShareWay(button->getTag());
	}
}

void ShareAwardPanel::setSelectShareWay(int index)
{
	_selected = index;
	updateIntro();
}

void ShareAwardPanel::onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		destroy();
	}
}

void ShareAwardPanel::updateIntro()
{
	auto name = "shareReward" + cocos2d::Value(_selected).asString();
	ValueVector rewards;
	ValueVector rewardVec;
	stringToValueVector(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", name.c_str()),"###",rewardVec);
	for(auto pair:rewardVec)
	{
		ValueMap rewardMap;
		stringToValueMap(pair.asString(), rewardMap);
		rewards.push_back(cocos2d::Value(rewardMap));
	}

	auto panelItem = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_item"));
	auto imageItem = static_cast<ImageView*>(Helper::seekWidgetByName(panelItem, "Image_item"));
	auto imageNum = static_cast<TextAtlas*>(Helper::seekWidgetByName(panelItem, "AtlasLabel"));

	auto isGet = static_cast<TextAtlas*>(Helper::seekWidgetByName(_root, "Label_isGet"));
	auto user = Detect::shareDetect()->getUser();
	isGet->setVisible(false);
	if(user->getShareRewardsLock(_selected - 1) == 1)
	{
		isGet->setVisible(true);
	}

	// 无奖品
	if(rewards.size() == 0)
	{
		imageNum->setString("");
	}
	else
	{
		// 默认只有一个奖品
		auto rewardMap = rewards.at(0).asValueMap();
		int typeId = rewardMap["typeId"].asInt();
		int num = rewardMap["num"].asInt();

		auto item = Item::create(typeId);

		//item->setPosition(TILE_HALF_WIDTH, TILE_HALF_HEIGHT);
		auto view = item->getView();
		view->retain();
		view->removeFromParent();
		view->setAnchorPoint(Vec2(0.16,0));
		imageItem->removeAllChildren();
		imageItem->addChild(view);
		view->release();

		imageNum->setString(cocos2d::Value(num).asString());
	}
}

// 分享
void ShareAwardPanel::onShare(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		//确定购买
		JniMethodInfo jmi;
		if(JniHelper::getStaticMethodInfo(jmi ,"IAP/IAPJni" ,"shareOrder" ,"(ILjava/lang/String;)V"))
		{
			jstring jstr = jmi.env->NewStringUTF(a2u("我在玩《魔塔天城》，分享就获得两把钥匙，一起来解迷吧？ 下来看看：").c_str());
			jmi.env->CallStaticVoidMethod(jmi.classID , jmi.methodID, _selected, jstr);
		}
#else
		//shareSuccess(_selected);
		auto user = Detect::shareDetect()->getUser();
		user->getShareReward(_selected);
#endif
	}
}

// 授权
void ShareAwardPanel::onAuth(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
//	if (touchType == Widget::TouchEventType::ENDED) 
//	{
//#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
//		JniMethodInfo minfo;
//		bool isHave = JniHelper::getStaticMethodInfo(minfo,
//			"IAP/IAPJni",
//			"getJavaActivity",
//			"()Ljava/lang/Object;");
//		jobject activityObj;
//		if (isHave)
//		{
//			//调用静态函数getJavaActivity，获取java类对象。
//			activityObj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
//		}
//		//2. 查找displayWebView接口，获取其函数信息，并用jobj调用
//		isHave = JniHelper::getMethodInfo(minfo,"IAP/IAPJni","authOrder", "(I)V"); 
//
//		if (!isHave)
//		{
//			CCLog("jni:authOrder 函数不存在");
//		}
//		else
//		{
//			//调用displayWebView函数，并传入参数
//			minfo.env->CallVoidMethod(activityObj, minfo.methodID, _selected);
//		}
//#else
//		//shareSuccess(_selected);
//#endif
//	}
}
