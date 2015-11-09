#ifndef __FIGHT_H__
#define __FIGHT_H__

#include "cocos2d.h"
#include "cell/actor/Actor.h"
#include "cell/effect/Effect.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "utils/Shake.h"
USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class Fight:public Node
{
public:
	Fight(std::string oneName, std::string twoName);
	static Fight* create(std::string oneName, std::string twoName);
private:
	Actor* _one;
	Actor* _two;
	Actor* _first;
	Actor* _second;
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
	Shake* _shake1;
	Shake* _shake2;
	Action* _tintred1;
	Action* _tintred2;
	bool _end;
private:
	void start();
	void end();
	void check1();
	void check2();
	void start2(float dt);
};

#endif /*__FIGHT_H__*/