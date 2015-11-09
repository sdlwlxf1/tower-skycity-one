#ifndef __FIGHT_NEW_H__
#define __FIGHT_NEW_H__

#include "cocos2d.h"
#include "cell/actor/Actor.h"
#include "cell/effect/Effect.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "utils/Shake.h"
USING_NS_CC;
using namespace cocostudio;
using namespace ui;


//class Timer : public Ref
//{
//public:
//	Timer(int delay);
//	static Timer* create(int delay);
//private:
//	TimerContainer *_container;
//	int _count;
//	int _delay;
//};
//
//class TimerContainer : public Ref
//{
//public:
//	TimerContainer(float interval);
//	static TimerContainer* create(float interval);
//	void registerTimer(Timer* timer);
//	float getInterval(){return _interval;}
//private:
//	float _interval;
//};

class FightNew:public Node
{
public:
	typedef struct Fight_Param
	{
		// ÉËº¦½á¹û
		int hit;
		int hitNav;
		int attAddHp;
		int defAddHp;
		bool missHit;
		bool doubleHit;
		std::vector<Actor::FightBuff> attFB;
		std::vector<Actor::FightBuff> defFB;
	} FightParam;
	FightNew(std::string oneName, std::string twoName);
	static FightNew* create(std::string oneName, std::string twoName);
private:
	Actor* _one;
	Actor* _two;
	Actor* _first;
	Actor* _second;
	Actor* _winer;
	Actor* _loser;
	Action* _action1;
	Action* _action2;
	RepeatForever* _repeat1;
	RepeatForever* _repeat2;
	Label* _hitHp1;
	Label* _hitHp2;
	Effect* _effect1;
	Effect* _effect2;
	Effect* _effect3;
	Effect* _effect4;
	Shake* _shake;
	Action* _tintred;
	bool _end;
	int _turn;
	int _fightLast;
	int _fightCount;
	float _roundDelay;
private:
	void hitText();
	void updateFight(float delta);
	void start();
	void end();
	void check1();
	void check2();
	void start2(float dt);
	void fight(Actor* attacker, Actor* defender);
	float fightEffect(Actor* attacker, Actor* defender, FightParam fightParam);
};

#endif /*__FIGHT_NEW_H__*/