#include "AchievementPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "ui/UIScrollView.h"
#include "Global.h"
#include "cell/CellFactory.h"
#include "ui/PopPanel.h"
#include "Detect.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform\android\jni\JniHelper.h"
#endif

#define NONE_IMAGE "none.png"

AchievementPanel* AchievementPanel::create(std::string name)
{
	AchievementPanel *pRet = new AchievementPanel(name);
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

void AchievementPanel::selectedItemEvent(Ref *pSender, ListView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
			//auto tag = listView->getItem(listView->getCurSelectedIndex())->getTag();
			//auto endStatUnit = _endStatsUnit.at(listView->getCurSelectedIndex());
			break;
		}
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child end index = %ld", listView->getCurSelectedIndex());

			auto tag = _endStatUnits.at(listView->getCurSelectedIndex())->getTag();
			auto model = _model.at(tag);
			auto endStatPan = EndStatPanel::create("endStat");
			endStatPan->initWithModel(model);

			PopPanel::getInstance()->addPanel(endStatPan, 2);
			break;
		}
	default:
		break;
	}
}

void AchievementPanel::selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type)
{
	switch (type) {
	case ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
		CCLOG("SCROLL_TO_BOTTOM");
		break;
	case ui::ScrollView::EventType::SCROLL_TO_TOP:
		CCLOG("SCROLL_TO_TOP");
		break;
	default:
		break;
	}
}

bool AchievementPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(AchievementPanel::onButtonClicked, this));
	}
	
	//_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));
	_listView = static_cast<ListView*>(Helper::seekWidgetByName(_root, "ListView_content"));
	_endNumLab = static_cast<Text*>(Helper::seekWidgetByName(_root, "Label_end_num"));

	_endStatUnit = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_endStatUnit"));
	_endStatUnit->retain();
	_endStatUnit->removeFromParent();

	_listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(AchievementPanel::selectedItemEvent, this));
	_listView->addEventListener((ui::ListView::ccScrollViewCallback)CC_CALLBACK_2(AchievementPanel::selectedItemEventScrollView,this));
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	setKeyboardEnabled(true);
	return true;
}

void AchievementPanel::initWithModel(std::vector<ValueMap> model)
{
	_model = model;
	int i = model.size() - 1;
	int j = 0;
	for(;i >= 0; i--,j++)
	{
		auto pair = model.at(i);
		auto statModel = pair;
		auto endStatUnit = _endStatUnit->clone();
		
		auto tagLab = static_cast<Text*>(Helper::seekWidgetByName(endStatUnit, "Label_tag"));
		auto timeLab = static_cast<Text*>(Helper::seekWidgetByName(endStatUnit, "Label_time"));
		/*auto removeBtn = static_cast<Button*>(Helper::seekWidgetByName(endStatUnit, "Button_remove"));
		removeBtn->addTouchEventListener(CC_CALLBACK_2(AchievementPanel::onRemoveButtonClicked, this));
		removeBtn->setTag(j);*/

		tagLab->setString(pair["endTag"].asString());
		timeLab->setString(pair["curTime"].asString());

		endStatUnit->setTag(i);
		_endStatUnits.pushBack(endStatUnit);
		_listView->pushBackCustomItem(endStatUnit);
	}

	_endNumLab->setString(cocos2d::Value((int)model.size()).asString());
}

void AchievementPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			destroy();
			break;
		}
	}
}

void AchievementPanel::onRemoveButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		int tag = button->getTag();

		_listView->removeItem(tag);
		_endStatUnits.erase(tag);
		//Detect::shareDetect()->getUser()->getAchieves().erase(button->getParent()->getTag());
	}
}

void AchievementPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}