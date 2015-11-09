#ifndef __USER_H__
#define __USER_H__

#include "cocos2d.h"
#include "buff/BuffTarget.h"

USING_NS_CC;

class User : public Ref , public BuffTarget
{
public:
	~User();
	static User* create();
	bool init();
	void setModelByName(std::string name, cocos2d::Value value);
	cocos2d::Value getModelByName(std::string name);

	void reset();

	void pushAchieve(ValueMap model);
	void removeAchieve(int index);
	ValueMap getAchieve(int index);
	ValueMap getLastAchieve();

	std::vector<ValueMap> getAchieves();
	ValueMap saveModel();
	void loadModel(ValueMap model);
	

	void addBuff(Buff* buff);
	void removeBuff(Buff* buff);
	void updateBuffsEffect();
	Buff* getBuffByTypeId(int typeId);

	Vector<Buff*> getBuffs(){return _buffs;}

	int getShareRewardsLock(int index){return _shareRewardsLock[index];}
	void setShareRewardsLock(int index, int value);
	void getShareReward(int index);

	CC_SYNTHESIZE(float, _multiSpeed, MultiSpeed);
	CC_SYNTHESIZE(float, _multiFightDelay, MultiFightDelay);

	CC_SYNTHESIZE(bool, _firstOpen, FirstOpen);

	bool getShareRewardOpen(){return _shareRewardOpen;}
	void setShareRewardOpen(bool value);
	CC_SYNTHESIZE(bool, _shareRewardOpenFirst, ShareRewardOpenFirst);
private:
	std::string _path;
	ValueMap _model;
	std::vector<ValueMap> _achieves;
	bool _shareRewardOpen;
	int _shareRewardsLock[3];
};

#endif