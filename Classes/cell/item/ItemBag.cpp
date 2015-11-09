#include "cell/item/ItemBag.h"
#include "cell/CellFactory.h"
#include "cell/actor/Actor.h"
#include "cell/Cell.h"

void ItemBag::addEquip(Equip* equip)
{
	_equips.insert(equip->getTypeId1(),equip);
	updateAddData();
}

void ItemBag::removeEquip(Equip* equip)
{
	_equips.erase(_equips.find(equip->getTypeId1()));
	updateAddData();
}

void ItemBag::addEquipByType(int typeId)
{
	ValueMap model;
	model["typeId"] = typeId;
	auto equip = Equip::create(model);
	addEquip(equip);
	updateAddData();
}

void ItemBag::removeEquipByType(int typeId)
{
	auto remove = _equips.at(typeId);
	removeEquip(remove);
	updateAddData();
}

Equip* ItemBag::getEquipByType(int typeId)
{
	return _equips.at(typeId);
}

void ItemBag::updateAddData()
{
	auto temStr = _addStr;
	auto temDef = _addDef;
	_addStr = 0;
	_addDef = 0;
	float temAddFightBuffRate[10];
	for(int i=0; i < 10; i++)
	{
		temAddFightBuffRate[i] = _addFightBuffRate[i];
	}
	float temAddFightBuffDrate[10];
	for(int i=0; i < 10; i++)
	{
		temAddFightBuffDrate[i] = _addFightBuffDrate[i];
	}

	for(auto pair:_equips)
	{
		auto equip = pair.second;
		auto effect = equip->getEffectMap();
		for(auto pair:effect)
		{
			if(pair.first == "str")
			{
				_addStr += pair.second.asInt();
			}
			else if(pair.first == "def")
			{
				_addDef += pair.second.asInt();
			}
			else
			{
				for(int i = 0;i < 10;i ++)
				{
					if(pair.first == Actor::FightBuffRateStr[i])
					{
						_addFightBuffRate[i] += pair.second.asFloat();
					}
				}
				for(int i = 0;i < 10;i ++)
				{
					if(pair.first == Actor::FightBuffDrateStr[i])
					{
						_addFightBuffDrate[i] += pair.second.asFloat();
					}
				}
			}
		}
	}

	if(_owner == nullptr)
		return;

	if(temStr != _addStr)
	{
		_owner->addStr(0);
	}

	if(temDef != _addDef)
	{
		_owner->addDef(0);
	}

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

Prop* ItemBag::getPropByType(int typeId)
{
	if(getPropSizeByType(typeId) > 0)
	{
		return _props[typeId].at(_props[typeId].size()-1);
	}
	else
	{
		return nullptr;
	}	
}

void ItemBag::addProp(Prop* prop)
{
	auto typeId = prop->getTypeId();
	_props[typeId].pushBack(prop);
	tidy();
}

void ItemBag::addPropByType(int typeId, int num)
{
	while(num--)
	{
		ValueMap model;
		model["typeId"] = typeId;
		auto prop = Prop::create(model);
		addProp(prop);
	}
	if(num == -1)
		tidy();
}

void ItemBag::removeProp(Prop* prop)
{
	_props[prop->getTypeId()].eraseObject(prop);
	tidy();
}

void ItemBag::removePropByType(int typeId, int num)
{
	while(num--)
	{
		auto prop = getPropByType(typeId);
		if(prop != nullptr)
		{
			removeProp(prop);
		}
		else
		{
			break;
		}
	}	
}

void ItemBag::tidy()
{
	for(auto it = _props.begin();it != _props.end();)
	{
		if(it->second.size() <= 0)
		{
			if(_owner != nullptr)
				conditions[_owner->getName() + "_has_prop_type_" + cocos2d::Value(it->first).asString()] = LuaValue::intValue(0);
			it = _props.erase(it);
		}
		else
		{
			if(_owner != nullptr)
				conditions[_owner->getName() + "_has_prop_type_" + cocos2d::Value(it->first).asString()] = LuaValue::intValue(1);
			++it;
		}
	}

	for(auto its = _specials.begin();its != _specials.end();)
	{
		if(its->second.size() <= 0)
		{
			conditions[_owner->getName() + "_has_special_type_" + cocos2d::Value(its->first).asString()] = LuaValue::intValue(0);
			its = _specials.erase(its);
		}
		else
		{
			conditions[_owner->getName() + "_has_special_type_" + cocos2d::Value(its->first).asString()] = LuaValue::intValue(1);
			++its;
		}
	}
}

int ItemBag::getSpecialSizeByType(int typeId)
{
	return _specials[typeId].size();
}

Special* ItemBag::getSpecialByType(int typeId)
{
	if(getSpecialSizeByType(typeId) > 0)
	{
		return _specials[typeId].at(_specials[typeId].size()-1);
	}
	else
	{
		return nullptr;
	}	
}

void ItemBag::addSpecial(Special* special)
{
	auto typeId = special->getTypeId();
	_specials[typeId].pushBack(special);
}

void ItemBag::addSpecialByType(int typeId, int num)
{
	while(num--)
	{
		ValueMap model;
		model["typeId"] = typeId;
		auto special = Special::create(model);
		addSpecial(special);
	}	
}

void ItemBag::removeSpecial(Special* special)
{
	_specials[special->getTypeId()].eraseObject(special);
	tidy();
}

void ItemBag::removeSpecialByType(int typeId, int num)
{
	while(num--)
	{
		auto special = getSpecialByType(typeId);
		if(special != nullptr)
		{
			removeSpecial(special);
		}
		else
		{
			break;
		}
	}	
}

int ItemBag::getGold()
{
	return _gold;
}

void ItemBag::setGold(int gold)
{
	_gold = gold;
}

void ItemBag::addGold(int gold)
{
	auto value = _gold + gold;
	setGold(value);
}

int ItemBag::getCapacity()
{
	return 100;
}

int ItemBag::getCount()
{
	return 1;
}

int ItemBag::getPropSizeByType(int typeId)
{
	return _props[typeId].size();
}

ItemBag::ItemBag()
{

}

ItemBag::ItemBag(ValueMap &model)
{
	_model = model;
}

//~Item();
ItemBag* ItemBag::create()
{
	ItemBag *pRet = new ItemBag();
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

ItemBag* ItemBag::create(ValueMap &model)
{
	ItemBag *pRet = new ItemBag(model);
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

bool ItemBag::init()
{
	_owner = nullptr;
	initModel();
	return true;
}

void ItemBag::setOwner(Actor* owner)
{
	_owner = owner;
	tidy();
	updateAddData();
}

Actor* ItemBag::getOwner()
{
	return _owner;
}

// override
void ItemBag::initModel()
{
	auto equipsV = _model["equips"];
	if(!equipsV.isNull())
	{
		for(auto model:equipsV.asValueVector())
		{
			addEquip(Equip::create(model.asValueMap()));
		}
	}
	auto propsV = _model["props"];
	if(!propsV.isNull())
	{
		for(auto model:propsV.asValueVector())
		{
			addProp(Prop::create(model.asValueMap()));
		}
	}
	auto specialsV = _model["specials"];
	if(!specialsV.isNull())
	{
		for(auto model:specialsV.asValueVector())
		{
			addSpecial(Special::create(model.asValueMap()));
		}
	}

	for(int i=0;i<10;i++)
	{
		_addFightBuffRate[i] = 0;
	}
	for(int i=0;i<10;i++)
	{
		_addFightBuffDrate[i] = 0;
	}
	_gold = 0;
	_addHp = 0;
	_addStr = 0;
	_addDef = 0;
}

void ItemBag::loadModel(ValueMap &map)
{
	Model::modelCoverModel(map, _model);
}

ValueMap ItemBag::saveModel()
{
	ValueMap model;
	ValueVector equips;
	for(auto pair:_equips)
	{
		equips.push_back(cocos2d::Value(pair.second->saveModel()));
	}
	ValueVector props;
	for(auto pair:_props)
	{
		auto vec = pair.second;
		for(auto prop:vec)
		{
			props.push_back(cocos2d::Value(prop->saveModel()));
		}
	}
	ValueVector specials;
	for(auto pair:_specials)
	{
		auto vec = pair.second;
		for(auto special:vec)
		{
			specials.push_back(cocos2d::Value(special->saveModel()));
		}
	}
	model["equips"] = equips;
	model["props"] = props;
	model["specials"] = specials;
	return model;
}

const std::string ItemBag::getNameId()
{
	return _model["name"].asString();
}

bool ItemBag::onTrigger(EventCustom *event)
{
	return false;
}
