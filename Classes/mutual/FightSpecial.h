#ifndef __FIGHT_SPECIAL_H__
#define __FIGHT_SPECIAL_H__

#include "cocos2d.h"
#include "cell/actor/Actor.h"
#include "cell/effect/Effect.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "utils/Shake.h"
USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class FightSpecialUnit:public Layer
{
public:
	enum RES
	{
		BAD,GOOD,PERFECT
	};
	FightSpecialUnit();
	static FightSpecialUnit* create();
	void initBy();
	void begin();
	void end();
	void last();
	std::function<void(RES result)> callBack;
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
protected:
	void onDraw(const Mat4 &transform, uint32_t flags);
	CustomCommand _customCommand;
private:
	FightSpecialUnit::RES check();
	void update(float delay);
	void drawRing();
	DrawNode* _ring;
	Sprite* _sprite;
	float _radius;
	Point _perfectRate;
	Point _goodRate;
	Point _extentRate;
	float _ringMaxRadius;
	float _ringMinRadius;
	float _ringCurRadius;
	float _ringPerRadius;
};

class FightSpecial:public Ref
{
public:
	FightSpecial(std::string oneName, std::string twoName);
	static FightSpecial* create(std::string oneName, std::string twoName);
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

#endif /*__FIGHT_SPECIAL_H__*/