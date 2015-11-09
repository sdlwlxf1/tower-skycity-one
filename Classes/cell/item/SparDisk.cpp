#include "cell/item/SparDisk.h"
#include "cell/CellFactory.h"
#include "cell/actor/Actor.h"
#include "cell/Cell.h"

void SparDisk::equipSpar(int index, int typeId)
{
	_equipSpars.insert(index,_spars.at(typeId));
	takeOutSpar(typeId);
	updateAddData();
}

void SparDisk::getOffSpar(int index)
{
	auto spar = _equipSpars.at(index);
	_equipSpars.erase(index);
	bagSpar(spar->getTypeId());
	updateAddData();
}

void SparDisk::bagSpar(int typeId)
{
	_bagSpars.insert(typeId, _spars.at(typeId));
}

void SparDisk::addSpar(Spar* spar)
{
	if(_spars.find(spar->getTypeId()) == _spars.end())
	{
		_spars.insert(spar->getTypeId(),spar);
		bagSpar(spar->getTypeId());
	}
}


void SparDisk::addSparByType(int typeId)
{
	ValueMap model;
	model["typeId"] = typeId;
	auto spar = Spar::create(model);
	addSpar(spar);
	updateAddData();
}

Spar* SparDisk::getSpar(int typeId)
{
	return _spars.at(typeId);
}

void SparDisk::takeOutSpar(int typeId)
{
	_bagSpars.erase(typeId);
}

void SparDisk::updateAddData()
{
	float temAddFightBuffRate[10];
	for(int i=0; i < 10; i++)
	{
		temAddFightBuffRate[i] = _addFightBuffRate[i];
		_addFightBuffRate[i] = 0;
	}
	float temAddFightBuffDrate[10];
	for(int i=0; i < 10; i++)
	{
		temAddFightBuffDrate[i] = _addFightBuffDrate[i];
		_addFightBuffDrate[i] = 0;
	}
	
	for(auto pair:_equipSpars)
	{
		auto spar = pair.second;
		if(spar == nullptr) return;
		_addFightBuffRate[spar->getTypeId1()] += spar->getRate();
		_addFightBuffDrate[spar->getTypeId1()] += spar->getDrate();
	}

	if(_owner == nullptr)
		return;

	for(int i = 0;i < 10;i ++)
	{
		if(temAddFightBuffRate[i] != _addFightBuffRate[i])
		{
			_owner->addFightBuffRate(Actor::FightBuff(i),0);
		}
	}

	for(int i = 0;i < 10;i ++)
	{
		if(temAddFightBuffDrate[i] != _addFightBuffDrate[i])
		{
			_owner->addFightBuffDrate(Actor::FightBuff(i),0);
		}
	}
}

SparDisk::SparDisk()
{

}

SparDisk::SparDisk(ValueMap &model)
{
	_model = model;
}

//~Item();
SparDisk* SparDisk::create()
{
	SparDisk *pRet = new SparDisk();
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

SparDisk* SparDisk::create(ValueMap &model)
{
	SparDisk *pRet = new SparDisk(model);
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

bool SparDisk::init()
{
	_owner = nullptr;
	initModel();
	return true;
}

void SparDisk::setOwner(Actor* owner)
{
	_owner = owner;
	updateAddData();
}

Actor* SparDisk::getOwner()
{
	return _owner;
}

// override
void SparDisk::initModel()
{
	for(int i=0;i<10;i++)
	{
		_addFightBuffRate[i] = 0;
	}
	for(int i=0;i<10;i++)
	{
		_addFightBuffDrate[i] = 0;
	}

	auto sparsV = _model["spars"];
	if(!sparsV.isNull())
	{
		for(auto pair:sparsV.asValueVector())
		{
			addSpar(Spar::create(pair.asValueMap()));
		}
	}

	auto equipSparsV = _model["equipSpars"];
	if(!equipSparsV.isNull())
	{
		for(auto pair:equipSparsV.asValueMap())
		{
			equipSpar(cocos2d::Value(pair.first).asInt(),pair.second.asInt());
		}
	}
}

void SparDisk::loadModel(ValueMap &map)
{
	Model::modelCoverModel(map, _model);
}

ValueMap SparDisk::saveModel()
{
	ValueMap model;
	ValueVector spars;
	for(auto pair:_spars)
	{
		spars.push_back(cocos2d::Value(pair.second->saveModel()));
	}
	ValueMap equipSpars;
	for(auto pair:_equipSpars)
	{
		equipSpars[cocos2d::Value(pair.first).asString()] = pair.second->getTypeId();
	}
	model["spars"] = spars;
	model["equipSpars"] = equipSpars;
	return model;
}

const std::string SparDisk::getNameId()
{
	return _model["name"].asString();
}

bool SparDisk::onTrigger(EventCustom *event)
{
	return false;
}
