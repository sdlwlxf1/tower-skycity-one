#include "BubbleLabel.h"

BubbleLabel* BubbleLabel::create()
{
	BubbleLabel *ret = new BubbleLabel();	
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

BubbleLabel::BubbleLabel()
{

}

bool BubbleLabel::init()
{
	_bubbling = false;
	return true;
}

void BubbleLabel::addLabel(std::string info, Vec2 site, Color3B color)
{
	Label* label = Label::createWithBMFont("UI/Export/UI_1/UI/common/font/font2_0.fnt",info);
	label->setColor(color);
	label->enableShadow(Color4B::BLACK,Size(1,-1),5);
	label->setString(info);
	label->setPosition(Vec2(site.x,site.y + 20));
	label->setAnchorPoint(Vec2(0.35,0));
	_vec.pushBack(label);
	bubble();
}

void BubbleLabel::bubble()
{
	if(_bubbling == true)
		return;
	bubbleOutLabel();
}

void BubbleLabel::bubbleOutLabel()
{
	if(_vec.size() > 0)
	{
		_bubbling = true;
		auto label = _vec.front();
		label->setOpacity(100);
		addChild(label);
		auto site = label->getPosition();
		Vec2 vec1(site.x,site.y + 50);
		Vec2 vec2(site.x,site.y + 50);
		label->runAction(Sequence::create(DelayTime::create(0.3),CallFunc::create([=](){bubbleOutLabel();}),NULL));
		label->runAction(Sequence::create(MoveTo::create(1,vec2)/*,*/,NULL));
		label->runAction(Sequence::create(FadeIn::create(0.2),DelayTime::create(0.3),FadeOut::create(0.5),CallFuncN::create([](Node* node){node->removeFromParentAndCleanup(true);}),NULL));
		_vec.erase(0);
	}
	else
	{
		_bubbling = false;
	}
}