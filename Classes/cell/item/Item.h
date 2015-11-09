#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"
#include "TObject.h"
#include "cell/Cell.h"
USING_NS_CC;

class Item : public Cell
{
public:
	Item(int typeId);
	Item(ValueMap& model);
	//~Item();
	static Item* create(int typeId);
	static Item* create(ValueMap& model);
	int getTypeId();
	int getTypeId1();
	std::string getType();
	virtual ValueMap getEffectMap();
	virtual void addDurable(int num);
	int getDurable();
// override	
	virtual void onEnter();
	virtual int isBlock();
	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();
	//void setAnimation(AnimationType type);
protected:
	virtual void setDurable(int value);
	virtual void initView();
	virtual void refreshModel();
	virtual void initModel();
	virtual void initOthers();
	virtual bool onTrigger(EventCustom *event);
protected:
	int _durable;
	int _typeId;
	int _isBlock;
};
// 装备
class Equip : public Item
{
public:
	Equip(int typeId):Item(typeId){}
	Equip(ValueMap& model):Item(model){}
	static Equip* create(int typeId);
	static Equip* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();
};
// 消耗品
class Prop : public Item
{
public:
	Prop(int typeId):Item(typeId){}
	Prop(ValueMap& model):Item(model){}
	static Prop* create(int typeId);
	static Prop* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
};
// 特殊道具
class Special : public Item
{
public:
	Special(int typeId):Item(typeId){}
	Special(ValueMap& model):Item(model){}
	static Special* create(int typeId);
	static Special* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
};
// 晶石
class Spar : public Item
{
public:
	Spar(int typeId):Item(typeId){}
	Spar(ValueMap& model):Item(model){}
	static Spar* create(int typeId);
	static Spar* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
	CC_SYNTHESIZE(float, _rate, Rate);
	CC_SYNTHESIZE(float, _drate, Drate);

	CC_SYNTHESIZE(float, _maxRate, MaxRate);
	CC_SYNTHESIZE(float, _maxDrate, MaxDrate);

	CC_SYNTHESIZE(float, _upRate, UpRate);
	CC_SYNTHESIZE(float, _upDrate, UpDrate);

	CC_SYNTHESIZE(int, _costPatchNum, CostPatchNum);

	CC_SYNTHESIZE(std::string, _upConfirm, UpConfirm);

	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();

	void upLevel();
protected:
	void initOthers();
};
// 一次性使用道具
class Gem : public Item
{
public:
	Gem(int typeId):Item(typeId){}
	Gem(ValueMap& model):Item(model){}
	static Gem* create(int typeId);
	static Gem* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	std::string getType();
};
// 门
class Door : public Item
{
public:
	Door(int typeId):Item(typeId){}
	Door(ValueMap& model):Item(model){}
	static Door* create(int typeId);
	static Door* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	void initModel();
	void initOthers();
	void open();
	void close();
	std::string getType();
	~Door();
protected:
	virtual ValueMap saveModel();
private:
	int _isOpen;
	Action* _delayCall;
};
// 机关
class Switch : public Item
{
public:
	Switch(int typeId):Item(typeId){}
	Switch(ValueMap& model):Item(model){}
	static Switch* create(int typeId);
	static Switch* create(ValueMap& model);
	bool onTrigger(EventCustom *event);
	void initOthers();
	ValueMap saveModel();
	void on();
	void off();
	std::string getType();
	int isOn();
private:
	int _on;
};

#endif /*__ITEM_H__*/