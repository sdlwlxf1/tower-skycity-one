#include "FightNew.h"
#include "utils/StringHandler.h"
#include "UI/ItemBagPanel.h"
#include "Detect.h"
#include "cell/CellFactory.h"
#include "utils/PlotScript.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

FightNew::FightNew(std::string oneName, std::string twoName)
{
	
	_one = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(oneName));
	_two = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(twoName));

	_roundDelay = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fightNew","roundDelay")).asFloat();

	auto player1 = dynamic_cast<Player*>(_one);
	if(player1 != nullptr)
		_roundDelay = player1->getFightDelay();
	auto player2 = dynamic_cast<Player*>(_two);
	if(player2 != nullptr)
		_roundDelay = player2->getFightDelay();

	start();
}

FightNew* FightNew::create(std::string oneName, std::string twoName)
{
	FightNew *pRet = new FightNew(oneName, twoName);
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

void FightNew::start()
{
	_first = _one;
	_second = _two;
	_end = false;

	_first->setCanWalk(false);
	_second->setCanWalk(false);
	_first->setCanGuide(false);
	_second->setCanGuide(false);

	scheduleUpdate();

	this->scheduleOnce(schedule_selector(FightNew::updateFight),0.2f);

	_turn = 1;
}

void FightNew::updateFight(float delta)
{
	// Õ½¶·
	Actor* attacker;
	Actor* defender;
	if(_turn == 1)
	{
		attacker = _first;
		defender = _second;
	}
	else if(_turn == 2)
	{
		attacker = _second;
		defender = _first;
	}
	fight(attacker,defender);
}

void FightNew::fight(Actor* attacker, Actor* defender)
{
	_fightLast = _roundDelay;

	float attFBRate[10];
	float attFBDrate[10];
	float defFBRate[10];
	float defFBDrate[10];
	for(int i = 0; i < 10; i++)
	{
		attFBRate[i] = attacker->getFightBuffRate(Actor::FightBuff(i));
		attFBDrate[i] = attacker->getFightBuffDrate(Actor::FightBuff(i));
		defFBRate[i] = defender->getFightBuffRate(Actor::FightBuff(i));
		defFBDrate[i] = defender->getFightBuffDrate(Actor::FightBuff(i));
	}

	std::vector<Actor::FightBuff> attFB;
	std::vector<Actor::FightBuff> defFB;
	for(int i = 0; i < 5; i++)
	{
		float random = CCRANDOM_0_1();
		if(random < attFBRate[i])
		{
			attFB.push_back(Actor::FightBuff(i));
		}
	}

	for(int i = 5; i < 10; i++)
	{
		float random = CCRANDOM_0_1();
		if(random < defFBRate[i])
		{
			defFB.push_back(Actor::FightBuff(i));
		}
	}

	auto attStr = attacker->getStr();
	auto attDef = attacker->getDef();
	auto defStr = defender->getDef();
	auto defDef = defender->getDef();

	int hitNav = attStr - defDef;
	hitNav = hitNav < 0 ? 0:hitNav;
	int hit = attStr - defDef;
	bool doubleHit = false;
	bool missHit = false;
	int attAddHp = 0;
	int defAddHp = 0;

	std::string buffText;

	for(auto fb:attFB)
	{
		switch (fb)
		{
		case Actor::KB:
			{
				attStr *= 1 + attFBDrate[fb];
				hit = attStr - defDef;
				//buffText = "¿ñ±©";
			}
			break;
		case Actor::PJ:
			{
				defDef *= 1 + attFBDrate[fb];
				hit = attStr - defDef;
				//buffText = "ÆÆ¼×";
			}
			break;
		case Actor::BJ:
			{
				auto dam = attStr - defDef;
				dam = dam < 0 ? 0:dam;
				hit = dam * (1 + attFBDrate[fb]);
				//buffText = "±©»÷";
			}
			break;
		case Actor::XX:
			{
				auto dam = attStr - defDef;
				dam = dam < 0 ? 0:dam;
				attAddHp += dam * attFBDrate[fb];
				//buffText = "ÎüÑª";
			}
			break;
		case Actor::LJ:
			{
				doubleHit = true;
				//buffText = "Á¬»÷";
			}
			break;
		default:
			break;
		}
	}

	

	for(auto fb:defFB)
	{
		switch (fb)
		{
		case Actor::RD:
			{
				defDef *= 1 - attFBDrate[fb];
				hit = attStr - defDef;
				//buffText = "¿ñ±©";
			}
			break;
		case Actor::RH:
			{
				attStr *= 1 - attFBDrate[fb];
				hit = attStr - defDef;
				//buffText = "¿ñ±©";
			}
			break;
		case Actor::GD:
			{
				auto dam = attStr - defDef;
				dam = dam < 0 ? 0:dam;
				hit = dam * (1 - attFBDrate[fb]);
				//buffText = "¿ñ±©";
			}
			break;
		case Actor::FT:
			{
				auto dam = attStr - defDef;
				dam = dam < 0 ? 0:dam;
				attAddHp -= dam * attFBDrate[fb];
				//buffText = "¿ñ±©";
			}
			break;
		case Actor::SB:
			{
				missHit = true;
				//buffText = "¿ñ±©";
			}
			break;
		default:
			break;
		}
	}

	hit = hit < 0 ? 0:hit;

	FightParam fightParam = {hit, hitNav, attAddHp, defAddHp, missHit, doubleHit,attFB,defFB};
	auto fightDelay = fightEffect(attacker,defender,fightParam);
	
	if(_end == false)
	{
		// Èç¹ûÃ»ÓÐÒ»·½ËÀÍö£¬½øÐÐÏÂÒ»¹¥»÷
		this->runAction(Sequence::create(DelayTime::create(fightDelay),CallFunc::create([=](){updateFight(0);}),nullptr));
		_turn = _turn == 1?2:1;
	}
}

float FightNew::fightEffect(Actor* attacker, Actor* defender, FightParam fightParam)
{
	float fightDelay = 0;

	bool isBJ = false;
	bool isGD = false;
	bool isXX = false;
	bool isFT = false;

	auto shake = Shake::create(0.1f,3,0);
	shake->retain();

	/*auto tintredOne = TintBy::create(0.2f, 0, -255, -255);
	tintredOne->retain();
	auto tintred = Sequence::create(tintredOne,tintredOne->reverse(),nullptr);
	tintred->retain();

	auto tintGreenOne = TintBy::create(0.2f, -255, 0, -255);
	tintGreenOne->retain();
	auto tintGreen = Sequence::create(tintGreenOne,tintGreenOne->reverse(),nullptr);
	tintGreen->retain();

	auto toBig = ScaleBy::create(0.1f,1.2f,1.2f);
	toBig->retain();

	auto toSmall = ScaleBy::create(0.1f,0.8f,0.8f);
	toSmall->retain();*/

	for(auto pair:fightParam.attFB)
	{
		/*if(pair == Actor::FightBuff::KB)
		{
			attacker->setScale(1.15f);
		}
		else if(pair == Actor::FightBuff::PJ)
		{
			defender->setScale(0.85f);
		}*/
		if(pair == Actor::FightBuff::BJ)
		{
			isBJ = true;
		}
		else if(pair == Actor::FightBuff::XX)
		{
			isXX = true;
		}
	}

	for(auto pair:fightParam.defFB)
	{
		/*if(pair == Actor::FightBuff::RD)
		{
			defender->setScale(1.15f);
		}
		else if(pair == Actor::FightBuff::RH)
		{
			attacker->setScale(0.85f);
		}*/
		if(pair == Actor::FightBuff::GD)
		{
			isGD = true;
		}
		else if(pair == Actor::FightBuff::FT)
		{
			isFT = true;
		}
	}

	/*std::string attFBStr;
	std::string defFBstr;
	for(auto pair:fightParam.attFB)
	{
		attFBStr += a2u(Actor::FightBuffStr[(int)pair]) + "+";
	}
	if(attFBStr != "")
		attFBStr.pop_back();

	for(auto pair:fightParam.defFB)
	{
		defFBstr += a2u(Actor::FightBuffStr[(int)pair]) + "+";
	}
	if(defFBstr != "")
		defFBstr.pop_back();*/

	//Label* defFBLabel;
	//if(defFBstr != "")
	//{
	//	Label* defFBLabel = Label::createWithBMFont("UI/Export/UI_1/UI/common/font/font4_0.fnt",defFBstr);
	//	//attFBLabel->setColor();
	//	//defFBLabel->enableShadow(Color4B::BLACK,Size(1,-1),5);
	//	defFBLabel->setString(defFBstr);
	//	//attFBLabel->setPosition(Vec2(site.x,site.y + 20));
	//	defFBLabel->setAnchorPoint(Vec2(0.5,0.5));
	//	defFBLabel->setPosition(Vec2(40,40));
	//	attacker->addChild(defFBLabel);

	//	defender->runAction(shake);
	//	defender->runAction(tintGreen);

	//	auto scaleBy = ScaleBy::create(0.08f, 1.2f, 1.2f);
	//	defFBLabel->runAction(Sequence::create(scaleBy,scaleBy->reverse(),DelayTime::create(0.3f),CallFuncN::create([=](Node* node){defFBLabel->removeFromParentAndCleanup(true);}),NULL));
	//	fightDelay += 0.6f;
	//}

	//if(attFBStr != "")
	//{
	//	Label* attFBLabel = Label::createWithBMFont("UI/Export/UI_1/UI/common/font/font4_0.fnt",attFBStr);
	//	//attFBLabel->setColor();
	//	//attFBLabel->enableShadow(Color4B::BLACK,Size(1,-1),5);
	//	attFBLabel->setString(attFBStr);
	//	//attFBLabel->setPosition(Vec2(site.x,site.y + 20));
	//	attFBLabel->setAnchorPoint(Vec2(0.35,0));

	//	attFBLabel->setAnchorPoint(Vec2(0.5,0.5));
	//	attFBLabel->setPosition(Vec2(40,40));
	//	defender->addChild(attFBLabel);

	//	auto scaleBy = ScaleBy::create(0.08f, 1.2f, 1.2f);
	//	attFBLabel->runAction(Sequence::create(scaleBy,scaleBy->reverse(),DelayTime::create(0.3f),CallFuncN::create([=](Node* node){attFBLabel->removeFromParentAndCleanup(true);}),NULL));
	//}
	ValueMap value1;
	value1["typeId"] = 3001008;
	value1["retain"] = 1;
	auto effect1 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value1));
	effect1->setClearTime(1);
	Detect::shareDetect()->addCell(effect1,defender->getSite(),4,Detect::shareDetect()->getCurFloor()->getId(),Detect::shareDetect()->getCurTower()->getId());

	
	effect1->erupt();
	// Õ½¶·ÒôÐ§
	float random = CCRANDOM_0_1();
	if(random > 0.2)
	{
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:hit1");
	}
	else
	{
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:hit2");
	}
	if(isBJ)
	{
		ValueMap value2;
		value2["typeId"] = 3001009;
		value2["retain"] = 1;
		auto effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
		effect2->setClearTime(1);
		BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
		effect2->getView()->setBlendFunc(func);
		Detect::shareDetect()->addCell(effect2,defender->getSite(),4,Detect::shareDetect()->getCurFloor()->getId(),Detect::shareDetect()->getCurTower()->getId());
		effect2->erupt();
	}

	if(isGD)
	{
		ValueMap value3;
		value3["typeId"] = 3001009;
		value3["retain"] = 1;
		auto effect3 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value3));
		effect3->setClearTime(1);
		//BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
		//effect2->setBlendFunc(func);
		Detect::shareDetect()->addCell(effect3,defender->getSite(),4,Detect::shareDetect()->getCurFloor()->getId(),Detect::shareDetect()->getCurTower()->getId());
		effect3->erupt();
	}
	defender->runAction(shake);

	//auto hitHp2 = TextAtlas::create("0123456789", "UI/Export/UI_1/hud/GUIlabelatlasimg.png", 17, 22, "0");
	Label* hitHp;
	/*if(isCr == false)
	{*/
	hitHp = Label::createWithTTF("", "fonts/58983.ttf", 12);
	hitHp->enableOutline(Color4B(196,0,0,255),1);
	hitHp->setTextColor(Color4B::WHITE);
	hitHp->setAnchorPoint(Vec2(0.5, 0.5));
	defender->addChild(hitHp);
	
	//hitHp->setVisible(false);
	if(fightParam.missHit == false)
	{
		hitHp->setString(cocos2d::Value(-fightParam.hit).asString());
	}
	else
	{
		hitHp->setString("miss");
	}
	int r = -(TILE_HALF_WIDTH + 30) / 2 + CCRANDOM_0_1() * (TILE_HALF_WIDTH + 30);
	hitHp->runAction(Sequence::create(JumpTo::create(0.6, Vec2(r,20), 10, 1), CallFuncN::create([=](Node* node){
		node->setVisible(false);
		node->removeFromParentAndCleanup(true);
	}),NULL));
	auto scaleBy = ScaleBy::create(0.08f, 1.5f, 1.5f);
	hitHp->runAction(Sequence::create(scaleBy,scaleBy->reverse(),NULL));
	
	// ÉËº¦½á¹û
	if(fightParam.missHit == false)
	{
		// ÉËº¦½á¹û
		int defenderHp = defender->getHp();
		defenderHp -= fightParam.hit;
		attacker->addHp(fightParam.attAddHp);
		defenderHp += fightParam.defAddHp;

		if(defenderHp <= 0)
		{
			_winer = attacker;
			_loser = defender;
			_end = true;
		}
		else
		{
			defender->addHp(fightParam.defAddHp);
			defender->addHp(-fightParam.hit);
		}
	}
	

	if(fightParam.doubleHit == true)
	{
		std::vector<Actor::FightBuff> attFB;
		std::vector<Actor::FightBuff> defFB;
		FightParam fightParamDouble = {fightParam.hit, fightParam.hitNav, 0, 0, fightParam.missHit, false,attFB,defFB};
		this->runAction(Sequence::create(DelayTime::create(fightDelay / 2),CallFunc::create([=](){fightEffect(attacker,defender,fightParamDouble);}),nullptr));
	}
	if(_end == true)
	{
		end();
	}
	fightDelay = fightDelay < _roundDelay?_roundDelay:fightDelay;
	return fightDelay;
}

void FightNew::hitText()
{

}

void FightNew::end()
{
	this->unschedule(schedule_selector(FightNew::updateFight));
	this->stopAllActions();
	_winer->setCanWalk(true);
	_winer->setCanGuide(true, 0.5);
	if(dynamic_cast<Player*>(_winer) != nullptr)
	{
		auto player = dynamic_cast<Player*>(_winer);
		_winer->addXp(_loser->getXp());
		int addGold;
		if(player->getItemBag()->getPropByType(2002015) == nullptr)
		{
			addGold = _loser->getGold();
		}
		else
		{
			addGold = _loser->getGold() * 2;
		}
		_winer->addGold(addGold);
	}

	_loser->addHp(-_loser->getHp());
	this->removeFromParentAndCleanup(true);
	//runAction(Sequence::create(DelayTime::create(1), CallFunc::create([=](){}),nullptr));
}