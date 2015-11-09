#include "FightSpecial.h"
#include "utils/StringHandler.h"
#include "UI/ItemBagPanel.h"
#include "Detect.h"
#include "cell/CellFactory.h"
#include "utils//PlotScript.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

FightSpecialUnit::FightSpecialUnit()
{
}

static FightSpecialUnit* create()
{
	FightSpecialUnit *pRet = new FightSpecialUnit();
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
void FightSpecialUnit::initBy()
{
	//setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	_sprite = Sprite::createWithSpriteFrameName("other/circle.png");
	addChild(_sprite);

	_radius = _sprite->getContentSize().width * 0.5;

	// 创建一个事件监听器类型为 OneByOne 的单点触摸
	auto listener = EventListenerTouchOneByOne::create();
	// 设置是否吞没事件，在 onTouchBegan 方法返回 true 时吞没
	listener->setSwallowTouches(true);

	// 使用 lambda 实现 onTouchBegan 事件回调函数
	listener->onTouchBegan = [=](Touch* touch, Event* event){
		// 获取事件所绑定的 target 
		auto target = static_cast<Sprite*>(event->getCurrentTarget());

		// 获取当前点击点所在相对按钮的位置坐标
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		// 点击范围判断检测
		if (rect.containsPoint(locationInNode))
		{
			end();
			return true;
		}
		return false;
	};

	// 触摸移动时触发
	listener->onTouchMoved = [](Touch* touch, Event* event){};

	// 点击事件结束处理
	listener->onTouchEnded = [=](Touch* touch, Event* event){};
	
	// 添加监听器
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _sprite);

	_perfectRate = PointFromString(PlotScript::sharedHD()->getLuaVarOneOfTable("script/shop", "fightSpecial","specialRate"));
	_goodRate = PointFromString(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fightSpecial","goodRate"));
	_extentRate = PointFromString(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fightSpecial","_extentRate"));
	_ringPerRadius = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fightSpecial","_ringPerRadius")).asFloat();

	_ringMinRadius = _radius * _extentRate.x;
	_ringMaxRadius = _radius * _extentRate.y;
	_ringCurRadius = _ringMaxRadius;

	begin();
}

void FightSpecialUnit::update(float dt)
{
	Layer::update(dt);
	drawRing();
}

void FightSpecialUnit::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(FightSpecialUnit::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void FightSpecialUnit::onDraw(const Mat4 &transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	//draw
	CHECK_GL_ERROR_DEBUG();

	// draw a green circle with 50 segments with line to center
	glLineWidth(2);
	DrawPrimitives::setDrawColor4B(0, 255, 255, 255);
	DrawPrimitives::drawCircle(Vec2(0,0), 50, CC_DEGREES_TO_RADIANS(90), 50, true);

	CHECK_GL_ERROR_DEBUG();

	//end draw
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void FightSpecialUnit::drawRing()
{
	_ring->clear();
	if(_ringCurRadius < _ringMinRadius)
	{
		end();
	}
	_ringCurRadius += _ringPerRadius;
}

void FightSpecialUnit::begin()
{
	//this->schedule(schedule_selector(FightSpecialUnit::ringUpdate),0.1f);
	scheduleUpdate();
}

void FightSpecialUnit::end()
{
	//this->unschedule(schedule_selector(FightSpecialUnit::ringUpdate));
	RES result = check();
	switch (result)
	{
	case FightSpecialUnit::BAD:

		break;
	case FightSpecialUnit::GOOD:

		break;
	case FightSpecialUnit::PERFECT:

		break;
	default:
		break;
	}
	callBack(result);
}

FightSpecialUnit::RES FightSpecialUnit::check()
{
	auto curRate = _ringCurRadius / _radius;
	if(curRate > _perfectRate.x && curRate < _perfectRate.y)
	{
		return PERFECT;
	}
	else if(curRate > _goodRate.x && curRate < _goodRate.y)
	{
		return GOOD;
	}
	else
	{
		return BAD;
	}
}

FightSpecial::FightSpecial(std::string oneName, std::string twoName)
{
	_one = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(oneName));
	_two = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(twoName));
	start();
}

FightSpecial* FightSpecial::create(std::string oneName, std::string twoName)
{
	FightSpecial *pRet = new FightSpecial(oneName, twoName);
	if (pRet)
	{
		pRet->autorelease();
		pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}	
}

void FightSpecial::start()
{
	_first = _one;
	_second = _two;
	_end = false;

	_first->setCanWalk(false);
	_second->setCanWalk(false);
	_first->setCanGuide(false);
	_second->setCanGuide(false);

	//ValueMap value1;
	//value1["typeId"] = 3001008;
	//value1["retain"] = 1;
	//ValueMap value2;
	//value2["typeId"] = 3001008;
	//value2["retain"] = 1;
	//ValueMap value3;
	//value3["typeId"] = 3001009;
	//value3["retain"] = 1;
	//ValueMap value4;
	//value4["typeId"] = 3001009;
	//value4["retain"] = 1;
	//_effect1 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value1));
	//_effect1->setClearTime(1000);
	//_effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
	//_effect2->setClearTime(1000);
	//_effect3 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value3));
	//_effect3->setClearTime(1000);
	//_effect4 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value4));
	//_effect4->setClearTime(1000);
	//BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
	//_effect3->setBlendFunc(func);
	//_effect4->setBlendFunc(func);
	//Detect::shareDetect()->addCell(_effect1,_first->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	//Detect::shareDetect()->addCell(_effect2,_second->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	//Detect::shareDetect()->addCell(_effect3,_first->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	//Detect::shareDetect()->addCell(_effect4,_second->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());

	//_shake1 = Shake::create(0.1f,3,0);
	//_shake1->retain();
	//_shake2 = Shake::create(0.1f,3,0);
	//_shake2->retain();

	//auto tintred1 = TintBy::create(0.2f, 0, -255, -255);
	//_tintred1 = Sequence::create(tintred1,tintred1->reverse(),nullptr);
	//_tintred1->retain();
	//auto tintred2 = TintBy::create(0.2f, 0, -255, -255);
	//_tintred2 = Sequence::create(tintred2,tintred2->reverse(),nullptr);
	//_tintred2->retain();

	//_repeat1 = RepeatForever::create(Sequence::create(CallFunc::create(CC_CALLBACK_0(FightSpecial::check1,this)),DelayTime::create(0.5f),NULL));
	//_repeat2 = RepeatForever::create(Sequence::create(CallFunc::create(CC_CALLBACK_0(FightSpecial::check2,this)),DelayTime::create(0.5f),NULL));
	//_repeat1->retain();
	//_repeat2->retain();

	//runAction(_repeat1);
	///*_action2 = Sequence::create(DelayTime::create(0.25f), CallFunc::create([=](){_second->runAction(_repeat2);}),NULL);	
	//runAction(_action2);*/
	//this->scheduleOnce(schedule_selector(FightSpecial::start2), 0.25f); //在1.0f之后执行，并且只执行一次。 
}

void FightSpecial::start2(float dt)
{
	//runAction(_repeat2);
}

void FightSpecial::check1()
{
	//if(_end == true)
	//	return;
	//auto actor = _first;
	////log(actor->getName().c_str());
	///*SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//log( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek);*/

	//Actor* attacker = _first;
	//Actor* defender = _second;
	//// 暴击
	//bool isCr = false;
	//float random = CCRANDOM_0_1()*(100-1+1)+1;
	//if(random < attacker->getCrr()) isCr = true;
	//// 计算伤害
	//int hitNav = attacker->getStr() - defender->getDef();
	//hitNav = hitNav > 0? hitNav:0;
	//int hit = isCr ? (hitNav * (100+attacker->getCrdr()) * 0.01):hitNav;
	//// 伤害结果
	//int defenderHp = defender->getHp();
	//defenderHp -= hit;
	//Label* hitHp2;
	//if(isCr == false)
	//{
	//	hitHp2 = Label::createWithTTF("", "fonts/58983.ttf", 12);
	//	hitHp2->enableOutline(Color4B(196,0,0,255),1);
	//	hitHp2->setTextColor(Color4B::WHITE);
	//	_effect2->erupt();
	//}
	//else
	//{
	//	hitHp2 = Label::createWithTTF("", "fonts/58983.ttf", 16);
	//	hitHp2->enableOutline(Color4B(196,0,0,255),1);
	//	hitHp2->setTextColor(Color4B::YELLOW);
	//	_effect4->erupt();
	//	defender->stopAction(_shake2);
	//	defender->runAction(_shake2);
	//	defender->stopAction(_tintred2);
	//	defender->runAction(_tintred2);
	//}

	////auto hitHp2 = TextAtlas::create("0123456789", "UI/Export/UI_1/hud/GUIlabelatlasimg.png", 17, 22, "0");
	//_second->addChild(hitHp2);
	//hitHp2->setPosition(Vec2(30,40));
	//hitHp2->setAnchorPoint(Vec2(0.5, 0.5));
	//hitHp2->setVisible(true);
	//hitHp2->setString(cocos2d::Value(-hit).asString());
	////srand((unsigned)time(NULL));
	//int r = -TILE_HALF_HEIGHT + rand() % (TILE_HALF_HEIGHT+TILE_HALF_HEIGHT+1) + 30;
	//hitHp2->runAction(Sequence::create(JumpTo::create(0.6, Vec2(r,20), 10, 1), CallFuncN::create([](Node* node){node->setVisible(false);node->removeFromParentAndCleanup(true);}),NULL));

	//int defenderCp = defender->getXp();
	//int defenderGold = defender->getGold();
	//defender->addHp(-hit);
	//if(defenderHp <= 0)
	//{
	//	stopAllActions();
	//	_repeat1->release();
	//	_repeat2->release();
	//	attacker->setCanWalk(true);
	//	attacker->setCanGuide(true, 0.5);
	//	if(dynamic_cast<Player*>(attacker) != nullptr)
	//	{
	//		attacker->addXp(defenderCp);
	//		attacker->addGold(defenderGold);
	//	}
	//	end();
	//}
}

void FightSpecial::check2()
{
	//if(_end == true)
	//	return;
	//auto actor = _second;
	////log(actor->getName().c_str());
	///*SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//log( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek);*/
	//_effect1->erupt();
	//Actor* attacker = _second;
	//Actor* defender = _first;
	//// 暴击
	//bool isCr = false;
	//float random = CCRANDOM_0_1()*(100-1+1)+1;
	//if(random < attacker->getCrr()) isCr = true;
	//// 计算伤害
	//int hitNav = attacker->getStr() - defender->getDef();
	//hitNav = hitNav > 0? hitNav:0;
	//int hit = isCr ? (hitNav * (100+attacker->getCrdr()) * 0.01):hitNav;
	//// 伤害结果
	//int defenderHp = defender->getHp();
	//defenderHp -= hit;
	//Label* hitHp1;
	//if(isCr == false)
	//{
	//	hitHp1 = Label::createWithTTF("", "fonts/58983.ttf", 12);
	//	hitHp1->enableOutline(Color4B(196,0,0,255),1);
	//	hitHp1->setTextColor(Color4B::WHITE);
	//	_effect1->erupt();
	//}
	//else
	//{
	//	hitHp1 = Label::createWithTTF("", "fonts/58983.ttf", 15);
	//	hitHp1->enableOutline(Color4B(196,0,0,255),1);
	//	hitHp1->setTextColor(Color4B::YELLOW);
	//	_effect2->erupt();
	//	defender->stopAction(_shake1);
	//	defender->runAction(_shake1);
	//	defender->stopAction(_tintred1);
	//	defender->runAction(_tintred1);
	//}
	//hitHp1->setColor(Color3B::WHITE);
	//defender->addChild(hitHp1);
	//hitHp1->setPosition(Vec2(30,40));
	//hitHp1->setAnchorPoint(Vec2(0.5, 0.5));
	//hitHp1->setVisible(true);
	//hitHp1->setString(cocos2d::Value(-hit).asString());
	//int r = -TILE_HALF_HEIGHT + rand() % (TILE_HALF_HEIGHT+TILE_HALF_HEIGHT+1) + 30;
	//hitHp1->runAction(Sequence::create(JumpTo::create(0.6, Vec2(r,20), 10, 1), CallFuncN::create([](Node* node){node->setVisible(false);node->removeFromParentAndCleanup(true);}),NULL));

	//defender->addHp(-hit);
	//if(defenderHp <= 0)
	//{
	//	this->stopAllActions();
	//	_repeat1->release();
	//	_repeat2->release();
	//	attacker->setCanWalk(true);
	//	attacker->setCanGuide(true, 0.5);
	//	end();
	//}
}

void FightSpecial::end()
{
	//_effect1->removeFromFloor();
	//_effect2->removeFromFloor();
	//_effect3->removeFromFloor();
	//_effect4->removeFromFloor();
	//_shake1->release();
	//_tintred1->release();
	//_shake2->release();
	//_tintred2->release();
	//_end = true;
	//this->removeFromParentAndCleanup(true);
	////runAction(Sequence::create(DelayTime::create(1), CallFunc::create([=](){}),nullptr));
}