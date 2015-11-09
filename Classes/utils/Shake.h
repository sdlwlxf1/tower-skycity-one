//
//  CCShake.h
//
//  Code by Francois Guibert
//  Contact: www.frozax.com - http://twitter.com/frozax - www.facebook.com/frozax
//
//  何遵祖修改于2014.7.10 支持cocos2dx 3.0 修正了动作执行精灵位置错误问题 测试环境:cocos2d-x-3.0rc1

#ifndef __war__CCShake__
#define __war__CCShake__

#include "cocos2d.h"

using namespace cocos2d;

class Shake : public ActionInterval
{
public:
	Shake();

	// Create the action with a time and a strength (same in x and y)
	// 产生震动效果的初始化函数参数,两个方向相同
	// @param d 震动持续的时间
	// @param strength 震动的幅度
	static Shake* create(float d, float strength);
	// Create the action with a time and strengths (different in x and y)
	// 产生震动效果的初始化函数,两个方向值不一样
	static Shake* create(float d, float strength_x, float strength_y);
	bool initWithDuration(float d, float strength_x, float strength_y);

	//以下都是重写父类抽象类的函数(必须重写)
	virtual Shake* clone() const override;
	virtual Shake* reverse(void) const override;
	virtual void startWithTarget(Node *target) override;
	virtual void update(float time) override;
	virtual void stop(void);

protected:
	// Initial position of the shaked node
	// 精灵的位置
	float _initial_x, _initial_y;
	// Strength of the action
	// 抖动的幅度
	float _strength_x, _strength_y;
};

#endif /*__war__CCShake__*/