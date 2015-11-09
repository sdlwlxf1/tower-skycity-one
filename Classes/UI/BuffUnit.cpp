#include "BuffUnit.h"
#include "utils/StringHandler.h"

BuffUnit* BuffUnit::create()
{
	BuffUnit *pRet = new BuffUnit();
	if(pRet && pRet->init())
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

bool BuffUnit::init()
{
	return true;
}

void BuffUnit::setBuff(Buff* buff)
{
	_buff = buff;
	_buff->setBuffUnit(this);

	auto name = buff->getModelByName("image").asString();

	auto sprite = Sprite::createWithSpriteFrameName(name);
	sprite->setAnchorPoint(Vec2(0,0));
	addChild(sprite);

	auto endName = name;
	stringReplace(endName,".","_end.");
	auto endSprite = Sprite::createWithSpriteFrameName(endName);
	//endSprite->setAnchorPoint(Vec2(0,0));
	//endSprite->setBlendFunc()
	_endTimer = ProgressTimer::create(endSprite);
	_endTimer->setType(ProgressTimer::Type::RADIAL);
	_endTimer->setPosition(_endTimer->getContentSize().width / 2, _endTimer->getContentSize().height / 2);
	addChild(_endTimer);

	setContentSize(sprite->getContentSize());
	setTouchEnabled(true);
}

void BuffUnit::updateBuffTime(int curTime, int tolTime)
{
	auto rate = curTime * 1.0 / tolTime * 100;
	_endTimer->setPercentage(rate);
}

Buff* BuffUnit::getBuff()
{
	return _buff;
}

void BuffUnit::onExit()
{
	Layout::onExit();
	_buff->setBuffUnit(nullptr);
}