#include "Buff.h"
#include "Detect.h"
#include "utils/Tools.h"
#include "UI/BuffPanel.h"
#include "UI/BuffUnit.h"
#include "scene/GameScene.h"

Buff* Buff::create(int typeId)
{
	Buff *pRet = new Buff(typeId);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

Buff* Buff::create(ValueMap model)
{
	Buff *pRet = new Buff(model);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

Buff::Buff(int typeId)
{
	_typeId = typeId;
}

Buff::Buff(ValueMap model)
{
	_model = model;
	_typeId = model["typeId"].asInt();
}

bool Buff::init()
{
	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->buffDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->buffCsv->getModelById(typeId);
		Model::modelAddModel(model,_model);
	}

	_duration = -1;
	if(!_model["duration"].isNull())
	{
		_duration = _model["duration"].asFloat();
	}

	if(_duration != -1)
	{
		if(!_model["initTime"].isNull())
		{
			_initTime = _model["initTime"].asDouble();
		}
		else
		{
			_initTime = getTime();
		}
		Director::getInstance()->getScheduler()->schedule(schedule_selector(Buff::buffUpdate),this,1.0f,-1,0.0,false);
	}
	
	stringToValueMap(_model["effectStr"].asString(),_effectMap);

	_buffPanel = nullptr;
	_buffUnit = nullptr;
	return true;
}

void Buff::buffUpdate(float dt)
{
	auto curTime = getTime();
	auto durTime = difftime(curTime, _initTime);
	if(durTime > _duration)
	{
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(Buff::buffUpdate),this);
		if(_buffPanel != nullptr)
		{
			_buffPanel->destroy();
		}
		if(_buffUnit != nullptr)
		{
			setBuffUnit(nullptr);
		}
		removeFromTarget();
		return;
	}
	if(_buffPanel != nullptr)
	{
		_buffPanel->updateBuffTime(_duration - durTime);
	}
	if(_buffUnit != nullptr)
	{
		_buffUnit->updateBuffTime((int)durTime, (int)_duration);
	}
}

int Buff::getRemTime()
{
	auto curTime = getTime();
	auto durTime = difftime(curTime, _initTime);
	return _duration - durTime;
}

ValueMap Buff::saveModel()
{
	ValueMap model;
	model["typeId"] = _typeId;
	model["initTime"] = _initTime;
	return model;
}

void Buff::loadModel(ValueMap model)
{
	_model = model;
}

void Buff::removeFromTarget()
{
	if(_target != nullptr)
	{
		_target->removeBuff(this);
	}
}

void Buff::setModelByName(std::string name, cocos2d::Value value)
{
	_model[name] = value;
}

cocos2d::Value Buff::getModelByName(std::string name)
{
	return _model[name];
}