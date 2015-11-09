#include "ShopPanel.h"
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
#else
#include "JniTest.h"
#endif

#define NONE_IMAGE "none.png"

ShopPanel* ShopPanel::create(std::string name)
{
	ShopPanel *pRet = new ShopPanel(name);
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

void ShopPanel::selectedItemEvent(Ref *pSender, ListView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
			//auto tag = listView->getItem(listView->getCurSelectedIndex())->getTag();
			//auto goodUnit = _goodsUnit.at(listView->getCurSelectedIndex());

			break;
		}
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
		{
			ListView* listView = static_cast<ListView*>(pSender);
			CC_UNUSED_PARAM(listView);
			CCLOG("select child end index = %ld", listView->getCurSelectedIndex());
			auto goodUnit = _goodsUnit.at(listView->getCurSelectedIndex());
			auto good = goodUnit->getGood();
			PopPanel::getInstance()->confirm("buy","type:Text---text:"+ a2u("确定要花费 ") + cocos2d::Value(goodUnit->getPrice()).asString() + a2u("元 购买 ") + cocos2d::Value(goodUnit->getNum()).asString() + a2u("个") + good->getNickName() + a2u(" 吗？"),PromptPanel::TYPE_DONE_CANCEL,[=](){
				// 加速药水
				if(good->getTypeId() == 2003011 && Detect::shareDetect()->getUser()->getBuffByTypeId(4001001) != nullptr)
				{
					PopPanel::getInstance()->note("treaMap","type:Text---text:"+ a2u("已拥有该效果"),2.0f,false,true);
				}
				else
				{
					#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
					//确定购买
					JniMethodInfo jmi;
					if(JniHelper::getStaticMethodInfo(jmi ,"IAP/IAPJni" ,"orderGood" ,"(I)V"))
					{
						jmi.env->CallStaticVoidMethod(jmi.classID , jmi.methodID, listView->getCurSelectedIndex());
					}
					#else
					JniTest::getInstance()->orderSuccess(listView->getCurSelectedIndex());
					#endif
				}
			},nullptr,true,true);

			break;
		}
	default:
		break;
	}
}

void ShopPanel::selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type)
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

bool ShopPanel::init()
{
	/*for(auto node:_buttons->getChildren())
	{
	auto button = static_cast<Button*>(node);
	button->addTouchEventListener(CC_CALLBACK_2(ShopPanel::onButtonClicked, this));
	}*/
	
	//_container->setTouchEnabled(true);
	_container->setAnchorPoint(Vec2(0,0));
	_listView = static_cast<ListView*>(Helper::seekWidgetByName(_root, "ListView_content"));

	_goodPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_good"));
	_goodPan->retain();
	_goodPan->removeFromParent();

	_listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(ShopPanel::selectedItemEvent, this));
	_listView->addEventListener((ui::ListView::ccScrollViewCallback)CC_CALLBACK_2(ShopPanel::selectedItemEventScrollView,this));

	//_title = Label::create("","Arial",20);
	//_title->setColor(Color3B::BLACK);
	//_title->setPosition(0, _container->getSize().height / 2 - 30);
	//_container->addChild(_title);

	//_label = Label::create("","Arial",17);
	//_label->setColor(Color3B::BLACK);
	//_label->setLineBreakWithoutSpace(true);
	//_label->setAnchorPoint(Vec2(0,0));

	//_label->setDimensions(_scrollView->getSize().width,0);

	////_label->setPositionY(_scrollView->getSize().height / 2);
	////_label->setPositionX(_scrollView->getSize().width / 2);

	//_scrollView->addChild(_label);

	//auto clipper = ClippingNode::create();
	//clipper->setContentSize(_scrollView->getSize());
	////clipper->setPositionY(_scrollView->getSize().height / 2 + _scrollView->getPosition().y);
	////clipper->setAnchorPoint(Vec2(0.5, 0.5));
	//_container->addChild(clipper);

	//auto stencil = DrawNode::create();
	//Vec2 rectangle[4];
	//rectangle[0] = Vec2(-clipper->getContentSize().width / 2, -clipper->getContentSize().height / 2);
	//rectangle[1] = Vec2(-clipper->getContentSize().width / 2, clipper->getContentSize().height / 2);
	//rectangle[2] = Vec2(clipper->getContentSize().width / 2, clipper->getContentSize().height / 2);
	//rectangle[3] = Vec2(clipper->getContentSize().width / 2, -clipper->getContentSize().height / 2);

	//Color4F white(1, 1, 1, 1);
	//stencil->drawPolygon(rectangle, 4, white, 1, white);
	//clipper->setStencil(stencil);

	//_scrollView->retain();
	//_scrollView->removeFromParent();
	//clipper->addChild(_scrollView);
	//_scrollView->release();

	auto goodNum = cocos2d::Value(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", "goodsNum")).asInt();

	for(int i = 1; i <= goodNum; i++)
	{
		auto goodName = "good" + cocos2d::Value(i).asString();
		auto content = PlotScript::sharedHD()->getLuaVarString("script/Test.lua", goodName.c_str());
		ValueMap valueMap;
		stringToValueMap(std::string(content), valueMap);
		int lock = valueMap["lock"].asInt();
		if(conditions[goodName+"_unlock"].intValue() == 1)
			lock = 0;
		if(lock == 0)
		{
			auto goodPan = _goodPan->clone();
			auto goodUnit = GoodUnit::create(i);

			ValueMap model;
			model["typeId"] = valueMap["typeId"];
			auto good = CellFactory::getInstance()->createCell(model);
			goodUnit->setGood(good);
			goodUnit->setPrice(valueMap["price"].asInt());
			goodUnit->setNum(valueMap["num"].asInt());
			goodUnit->setView(goodPan);
			_goodsUnit.pushBack(goodUnit);

			goodPan->setTag(i);
			_listView->pushBackCustomItem(goodPan);
		}
	}
	
	_container->setScale(0.85f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));

	_root->setTouchEnabled(true);
	_container->setTouchEnabled(false);
	_root->addTouchEventListener(CC_CALLBACK_2(ShopPanel::onButtonClicked, this));
	setKeyboardEnabled(true);
	return true;
}

void ShopPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		destroy();
	}
}

void ShopPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		destroy();
	}
}


GoodUnit* GoodUnit::create(int id)
{
	GoodUnit *pRet = new GoodUnit(id);
	if (pRet)
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

GoodUnit::GoodUnit(int id)
{
	_id = id;
	_good = nullptr;
}

GoodUnit::~GoodUnit()
{
	_good->release();
}

void GoodUnit::setGood(Cell* good)
{
	_good = good;
	_good->retain();
}

void GoodUnit::setView(Widget* view)
{
	_view = view;
	updateView();
}

void GoodUnit::updateView()
{
	if(_good == nullptr)
		return;
	auto intro = static_cast<Text*>(Helper::seekWidgetByName(_view, "Label_intro"));
	auto name = static_cast<Text*>(Helper::seekWidgetByName(_view, "Label_name"));
	auto price = static_cast<Text*>(Helper::seekWidgetByName(_view, "Label_price"));
	
	auto goodPan = static_cast<Layout*>(Helper::seekWidgetByName(_view, "Panel_item"));
	auto button = static_cast<Button*>(Helper::seekWidgetByName(_view, "Button_item"));
	auto image = static_cast<ImageView*>(Helper::seekWidgetByName(_view, "Image_item"));
	auto imageBg = static_cast<ImageView*>(Helper::seekWidgetByName(_view, "Image_bg"));
	auto num = static_cast<TextAtlas*>(Helper::seekWidgetByName(_view, "AtlasLabel"));

	_view->setContentSize(imageBg->getContentSize());
	num->setString(cocos2d::Value(_num).asString());
	auto view = _good->getView();
	view->retain();
	view->removeFromParent();
	view->setAnchorPoint(Vec2(0.16,0));
	image->addChild(view);
	view->release();

	name->setString(_good->getNickName());
	intro->setString(_good->getModelByName("introduce").asString());
	price->setString(/*a2u("价格: ")+*/cocos2d::Value(_price).asString()+a2u("元"));
	
}