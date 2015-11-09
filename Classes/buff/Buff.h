#ifndef __BUFF_H__
#define __BUFF_H__

#include "cocos2d.h"

class BuffTarget;
class BuffUnit;
class BuffPanel;

USING_NS_CC;

class Buff : public Ref
{
public:
	Buff(int typeId);
	Buff(ValueMap model);
	static Buff* create(int typeId);
	static Buff* create(ValueMap model);
	bool init();
	void setModelByName(std::string name, cocos2d::Value value);
	cocos2d::Value getModelByName(std::string name);

	BuffTarget* getTarget(){return _target;}
	void setTarget(BuffTarget* target){_target = target;}

	ValueMap saveModel();
	void loadModel(ValueMap model);

	void removeFromTarget();

	ValueMap getEffectMap(){return _effectMap;}

	CC_SYNTHESIZE(int, _typeId, TypeId);
	CC_SYNTHESIZE(BuffPanel*, _buffPanel, BuffPanel);
	CC_SYNTHESIZE(BuffUnit*, _buffUnit, BuffUnit);

	int getRemTime();
private:
	void buffUpdate(float dt);

	ValueMap _model;

	BuffTarget* _target;
	float _duration;
	ValueMap _effectMap;

	double _initTime;
};

#endif