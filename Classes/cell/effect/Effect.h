#ifndef __EFFECT_H__
#define __EFFECT_H__

#include "cocos2d.h"
#include "cell/Cell.h"
USING_NS_CC;

class Effect :public Cell
{
public:
	Effect(int typeId){_typeId = typeId;}
	Effect(ValueMap& model);
	static Effect* create(int id);
	static Effect* create(ValueMap& model);
	std::string getType();
	void initModel();
	ValueMap saveModel();
	void loadModel(ValueMap &map);
	void initOthers();
	void initView();
	void erupt();
	int isBlock();
	int getZorder(){return _zorder;}
	void onEnter();
	void setClearTime(int count);
	void setRepeatTime(int count);

	int getTypeId1();
private:
	int _typeId;
	int _isBlock;
	int _zorder;
	bool _loop;
	int _repeatTime;
	int _clearTime;
	int _time;
	int _retain;

};

#endif /*__EFFECT_H__*/