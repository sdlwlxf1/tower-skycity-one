#include "BookPanel.h"
#include "PopPanel.h"
#include "Detect.h"

#define NONE_IMAGE "none.png"

BookLine* BookLine::create(Actor* actor, Layout* layout)
{
	BookLine *pRet = new BookLine(actor,layout);
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

BookLine::BookLine(Actor* actor, Layout* layout)
{
	auto image = static_cast<ImageView*>(Helper::seekWidgetByName(layout, "Image_image"));
	auto labelHp = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_hp"));
	auto labelStr = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_str"));
	auto labelDef = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_def"));
	auto labelXp = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_xp"));
	auto labelGold = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_gold"));
	auto labelDamage = static_cast<Text*>(Helper::seekWidgetByName(layout, "Label_damage"));

	image->loadTexture(actor->getImageUrl(),Widget::TextureResType::PLIST);
	labelHp->setString(cocos2d::Value(actor->getHp()).asString());
	labelStr->setString(cocos2d::Value(actor->getStr()).asString());
	labelDef->setString(cocos2d::Value(actor->getDef()).asString());
	labelGold->setString(cocos2d::Value(actor->getGold()).asString());
	labelXp->setString(cocos2d::Value(actor->getXp()).asString());
	auto player = Detect::shareDetect()->getPlayer();
	auto mhit = actor->getStr() - player->getDef();
	auto phit = player->getStr() - actor->getDef();
	mhit = mhit > 0 ? mhit:0;
	phit = phit > 0 ? phit:0;
	int mround = ((phit == 0) ? 10000000:ceil(actor->getHp() * 1.0 / phit));
	auto damage = mhit * (mround - 1);
	std::string damageStr = phit == 0 ? a2u("ÎÞ·¨¹¥»÷") : cocos2d::Value(damage).asString();
	labelDamage->setString(cocos2d::Value(damageStr).asString());
}

bool BookLine::init()
{
	return true;
}


BookPanel* BookPanel::create(std::string name)
{
	BookPanel *pRet = new BookPanel(name);
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

bool BookPanel::init()
{
	_gapV = 0;

	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(BookPanel::onButtonClicked, this));
	}

	_panelLine = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_line"));
	_panelLines = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_lines"));

	_panelLines->removeChild(_panelLine);
	_panelLine->retain();

	auto floor = static_cast<TextAtlas*>(Helper::seekWidgetByName(_root, "AtlasLabel_floor"));
	floor->setString(cocos2d::Value(Detect::shareDetect()->getCurFloor()->getId()).asString());

	auto actors = Detect::shareDetect()->getCurFloor()->getAllActorType();
	actors.erase(Detect::shareDetect()->getPlayer()->getTypeId());
	initWithActors(actors);

	return true;
}

bool sortActor(Cell* t1, Cell* t2)
{
	return t1->getTypeId() < t2->getTypeId();
}

void BookPanel::initWithActors(Map<int, Cell*> map)
{
	Vector<Cell*> actors;
	for(auto pair:map)
	{
		actors.pushBack(pair.second);
	}
	
	std::sort(actors.begin(),actors.end(),sortActor);

	int i = 0;
	for(auto cell:actors)
	{
		auto actor = dynamic_cast<Actor*>(cell);
		if(actor == nullptr) continue;
		BookLine *bookLine;
		auto panelLine = i == 0?_panelLine:static_cast<Layout*>(_panelLine->clone());

		bookLine = BookLine::create(actor, panelLine);
		_panelLines->addChild(panelLine);
		panelLine->setPosition(Vec2(0,-i * (_gapV + _panelLine->getSize().height)));
		i++;
	}
}

void BookPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED)
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			{
				destroy();
			}
			break;
		}

		//CC_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}