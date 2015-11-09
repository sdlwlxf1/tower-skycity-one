#include "Fight.h"
#include "utils/StringHandler.h"
#include "UI/ItemBagPanel.h"
#include "Detect.h"
#include "cell/CellFactory.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

Fight::Fight(std::string oneName, std::string twoName)
{
	_one = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(oneName));
	_two = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(twoName));
	start();
}

Fight* Fight::create(std::string oneName, std::string twoName)
{
	Fight *pRet = new Fight(oneName, twoName);
	if (pRet)
	{
		pRet->autorelease();
		//pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}	
}

void Fight::start()
{
	_first = _one;
	_second = _two;
	_end = false;

	_first->setCanWalk(false);
	_second->setCanWalk(false);
	_first->setCanGuide(false);
	_second->setCanGuide(false);

	ValueMap value1;
	value1["typeId"] = 3001008;
	value1["retain"] = 1;
	ValueMap value2;
	value2["typeId"] = 3001008;
	value2["retain"] = 1;
	ValueMap value3;
	value3["typeId"] = 3001009;
	value3["retain"] = 1;
	ValueMap value4;
	value4["typeId"] = 3001009;
	value4["retain"] = 1;
	_effect1 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value1));
	_effect1->setClearTime(1000);
	_effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
	_effect2->setClearTime(1000);
	_effect3 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value3));
	_effect3->setClearTime(1000);
	_effect4 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value4));
	_effect4->setClearTime(1000);
	BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
	_effect3->getView()->setBlendFunc(func);
	_effect4->getView()->setBlendFunc(func);
	Detect::shareDetect()->addCell(_effect1,_first->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	Detect::shareDetect()->addCell(_effect2,_second->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	Detect::shareDetect()->addCell(_effect3,_first->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());
	Detect::shareDetect()->addCell(_effect4,_second->getSite(),4,Detect::shareDetect()->getCurFloor()->getId());

	_shake1 = Shake::create(0.1f,3,0);
	_shake1->retain();
	_shake2 = Shake::create(0.1f,3,0);
	_shake2->retain();

	auto tintred1 = TintBy::create(0.2f, 0, -255, -255);
	_tintred1 = Sequence::create(tintred1,tintred1->reverse(),nullptr);
	_tintred1->retain();
	auto tintred2 = TintBy::create(0.2f, 0, -255, -255);
	_tintred2 = Sequence::create(tintred2,tintred2->reverse(),nullptr);
	_tintred2->retain();

	_repeat1 = RepeatForever::create(Sequence::create(CallFunc::create(CC_CALLBACK_0(Fight::check1,this)),DelayTime::create(0.5f),NULL));
	_repeat2 = RepeatForever::create(Sequence::create(CallFunc::create(CC_CALLBACK_0(Fight::check2,this)),DelayTime::create(0.5f),NULL));
	_repeat1->retain();
	_repeat2->retain();

	runAction(_repeat1);
	/*_action2 = Sequence::create(DelayTime::create(0.25f), CallFunc::create([=](){_second->runAction(_repeat2);}),NULL);	
	runAction(_action2);*/
	this->scheduleOnce(schedule_selector(Fight::start2), 0.25f); //在1.0f之后执行，并且只执行一次。 
}

void Fight::start2(float dt)
{
	runAction(_repeat2);
}

void Fight::check1()
{
	if(_end == true)
		return;
	auto actor = _first;
	//log(actor->getName().c_str());
	/*SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	log( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek);*/

	Actor* attacker = _first;
	Actor* defender = _second;
	//// 暴击
	bool isCr = false;
	//float random = CCRANDOM_0_1()*(100-1+1)+1;
	//if(random < attacker->getCrr()) isCr = true;
	// 计算伤害
	int hitNav = attacker->getStr() - defender->getDef();
	hitNav = hitNav > 0? hitNav:0;
	int hit = hitNav;//isCr ? (hitNav * (100+attacker->getCrdr()) * 0.01):hitNav;
	// 伤害结果
	int defenderHp = defender->getHp();
	defenderHp -= hit;
	Label* hitHp2;
	if(isCr == false)
	{
		hitHp2 = Label::createWithTTF("", "fonts/58983.ttf", 12);
		hitHp2->enableOutline(Color4B(196,0,0,255),1);
		hitHp2->setTextColor(Color4B::WHITE);
		_effect2->erupt();
	}
	else
	{
		hitHp2 = Label::createWithTTF("", "fonts/58983.ttf", 16);
		hitHp2->enableOutline(Color4B(196,0,0,255),1);
		hitHp2->setTextColor(Color4B::YELLOW);
		_effect4->erupt();
		defender->stopAction(_shake2);
		defender->runAction(_shake2);
		defender->stopAction(_tintred2);
		defender->runAction(_tintred2);
	}

	//auto hitHp2 = TextAtlas::create("0123456789", "UI/Export/UI_1/hud/GUIlabelatlasimg.png", 17, 22, "0");
	_second->addChild(hitHp2);
	hitHp2->setPosition(Vec2(30,40));
	hitHp2->setAnchorPoint(Vec2(0.5, 0.5));
	hitHp2->setVisible(true);
	hitHp2->setString(cocos2d::Value(-hit).asString());
	//srand((unsigned)time(NULL));
	int r = -TILE_HALF_HEIGHT + rand() % (TILE_HALF_HEIGHT+TILE_HALF_HEIGHT+1) + 30;
	hitHp2->runAction(Sequence::create(JumpTo::create(0.6, Vec2(r,20), 10, 1), CallFuncN::create([](Node* node){node->setVisible(false);node->removeFromParentAndCleanup(true);}),NULL));

	int defenderCp = defender->getXp();
	int defenderGold = defender->getGold();
	defender->addHp(-hit);
	if(defenderHp <= 0)
	{
		stopAllActions();
		_repeat1->release();
		_repeat2->release();
		attacker->setCanWalk(true);
		attacker->setCanGuide(true, 0.5);
		if(dynamic_cast<Player*>(attacker) != nullptr)
		{
			attacker->addXp(defenderCp);
			attacker->addGold(defenderGold);
		}
		end();
	}
}

void Fight::check2()
{
	if(_end == true)
		return;
	auto actor = _second;
	//log(actor->getName().c_str());
	/*SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	log( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek);*/
	_effect1->erupt();
	Actor* attacker = _second;
	Actor* defender = _first;
	// 暴击
	bool isCr = false;
	/*float random = CCRANDOM_0_1()*(100-1+1)+1;
	if(random < attacker->getCrr()) isCr = true;*/
	// 计算伤害
	int hitNav = attacker->getStr() - defender->getDef();
	hitNav = hitNav > 0? hitNav:0;
	int hit = hitNav;//isCr ? (hitNav * (100+attacker->getCrdr()) * 0.01):hitNav;
	// 伤害结果
	int defenderHp = defender->getHp();
	defenderHp -= hit;
	Label* hitHp1;
	if(isCr == false)
	{
		hitHp1 = Label::createWithTTF("", "fonts/58983.ttf", 12);
		hitHp1->enableOutline(Color4B(196,0,0,255),1);
		hitHp1->setTextColor(Color4B::WHITE);
		_effect1->erupt();
	}
	else
	{
		hitHp1 = Label::createWithTTF("", "fonts/58983.ttf", 15);
		hitHp1->enableOutline(Color4B(196,0,0,255),1);
		hitHp1->setTextColor(Color4B::YELLOW);
		_effect2->erupt();
		defender->stopAction(_shake1);
		defender->runAction(_shake1);
		defender->stopAction(_tintred1);
		defender->runAction(_tintred1);
	}
	hitHp1->setColor(Color3B::WHITE);
	defender->addChild(hitHp1);
	hitHp1->setPosition(Vec2(30,40));
	hitHp1->setAnchorPoint(Vec2(0.5, 0.5));
	hitHp1->setVisible(true);
	hitHp1->setString(cocos2d::Value(-hit).asString());
	int r = -TILE_HALF_HEIGHT + rand() % (TILE_HALF_HEIGHT+TILE_HALF_HEIGHT+1) + 30;
	hitHp1->runAction(Sequence::create(JumpTo::create(0.6, Vec2(r,20), 10, 1), CallFuncN::create([](Node* node){node->setVisible(false);node->removeFromParentAndCleanup(true);}),NULL));

	defender->addHp(-hit);
	if(defenderHp <= 0)
	{
		this->stopAllActions();
		_repeat1->release();
		_repeat2->release();
		attacker->setCanWalk(true);
		attacker->setCanGuide(true, 0.5);
		end();
	}
}

void Fight::end()
{
	_effect1->removeFromFloor();
	_effect2->removeFromFloor();
	_effect3->removeFromFloor();
	_effect4->removeFromFloor();
	_shake1->release();
	_tintred1->release();
	_shake2->release();
	_tintred2->release();
	_end = true;
	this->removeFromParentAndCleanup(true);
	//runAction(Sequence::create(DelayTime::create(1), CallFunc::create([=](){}),nullptr));
}