#include "PaperPanel.h"
#include "scene/StartScene.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "Global.h"

#define NONE_IMAGE "none.png"

PaperPanel* PaperPanel::create(std::string name)
{
	PaperPanel *pRet = new PaperPanel(name);
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

bool PaperPanel::init()
{
	/*for(auto node:_buttons->getChildren())
	{
	auto button = static_cast<Button*>(node);
	button->addTouchEventListener(CC_CALLBACK_2(PaperPanel::onButtonClicked, this));
	}*/
	
	//_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));
	_scrollView = static_cast<ScrollView*>(Helper::seekWidgetByName(_root, "ScrollView_content"));

	_title = Label::create("","Arial",20);
	_title->setColor(Color3B::BLACK);
	_title->setPosition(0, _container->getSize().height / 2 - 30);
	_container->addChild(_title);

	_label = Label::create("","Arial",17);
	_label->setColor(Color3B::BLACK);
	_label->setLineBreakWithoutSpace(true);
	_label->setAnchorPoint(Vec2(0,0));

	_label->setDimensions(_scrollView->getSize().width,0);

	//_label->setPositionY(_scrollView->getSize().height / 2);
	//_label->setPositionX(_scrollView->getSize().width / 2);

	_scrollView->addChild(_label);

	auto clipper = ClippingNode::create();
	clipper->setContentSize(_scrollView->getSize());
	//clipper->setPositionY(_scrollView->getSize().height / 2 + _scrollView->getPosition().y);
	//clipper->setAnchorPoint(Vec2(0.5, 0.5));
	_container->addChild(clipper);

	auto stencil = DrawNode::create();
	Vec2 rectangle[4];
	rectangle[0] = Vec2(-clipper->getContentSize().width / 2, -clipper->getContentSize().height / 2);
	rectangle[1] = Vec2(-clipper->getContentSize().width / 2, clipper->getContentSize().height / 2);
	rectangle[2] = Vec2(clipper->getContentSize().width / 2, clipper->getContentSize().height / 2);
	rectangle[3] = Vec2(clipper->getContentSize().width / 2, -clipper->getContentSize().height / 2);

	Color4F white(1, 1, 1, 1);
	stencil->drawPolygon(rectangle, 4, white, 1, white);
	clipper->setStencil(stencil);

	_scrollView->retain();
	_scrollView->removeFromParent();
	clipper->addChild(_scrollView);
	_scrollView->release();

	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	_root->addTouchEventListener(CC_CALLBACK_2(PaperPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}



void PaperPanel::initWithText(std::string text)
{
	//auto newText = stringLineBreakFormat(text,20);
	_label->setString(text);

	_label->setDimensions(_scrollView->getSize().width,0);

	auto size2 = _label->getContentSize();

	_scrollView->setInnerContainerSize(_label->getContentSize());
}

void PaperPanel::initWithTextNum(int num)
{
	_textId = num;
	auto name = "text"+cocos2d::Value(num).asString();
	auto titleName = "text"+cocos2d::Value(num).asString()+"Title";
	auto content = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "paperText",name.c_str());
	auto title = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "paperText",titleName.c_str());
	initWithText(content);
	_title->setString(title);
}

void PaperPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		/*auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{

		}*/
		auto name = "text" + cocos2d::Value(_textId).asString();
		conditions["close_" + name] = LuaValue::intValue(1);
		destroy();
	}
}

void PaperPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		auto name = "text" + cocos2d::Value(_textId).asString();
		conditions["close_" + name] = LuaValue::intValue(1);
		destroy();
	}
}
