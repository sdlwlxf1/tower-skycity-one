#include "CellFactory.h"
#include "actor/Actor.h"
#include "item/Item.h"
#include "effect/Effect.h"
#include "model/Model.h"
#include "Detect.h"
#include "editor-support/cocostudio/DictionaryHelper.h"

Cell* CellFactory::createCell(ValueMap& model)
{
	int typeId = model["typeId"].asInt();
	int modelType = typeId / 1000000;
	if(modelType < 1) return nullptr;
	std::string type;
	
	if(DATA_MODE == 1)
	{
		rapidjson::Document *datas = nullptr;
		if(modelType == 1)
		{
			datas = &(Model::shareModel()->actorDatas);
		}
		else if(modelType == 2)
		{
			datas = &(Model::shareModel()->itemDatas);
		}
		else if(modelType == 3)
		{
			datas = &(Model::shareModel()->effectDatas);
		}
		// 对象数据
		auto &data = Model::GetDataFromCol1(*datas,typeId);
		type = std::string(DICHELPER->getStringValue_json(data,"type"));
	}
	else if(DATA_MODE == 2)
	{
		CSVParse* models = nullptr;
		if(modelType == 1)
		{
			models = Model::shareModel()->actorCsv;
		}
		else if(modelType == 2)
		{
			models = Model::shareModel()->itemCsv;
		}
		else if(modelType == 3)
		{
			models = Model::shareModel()->effectCsv;
		}
		// 对象数据
		type = models->getValueByIdAndName(typeId, "type").asString();
	}
	
	if(model["name"].isNull() || type == "Effect")
	{
		model["name"] = type + "_random" + cocos2d::Value(cellCount++).asString();
	}
	
	if(type == "Player")
	{
		return Player::create(model);
	}
	else if(type == "Actor")
	{
		return Actor::create(model);
	}
	else if(type == "Monster")
	{
		return Monster::create(model);
	}
	else if(type == "Role")
	{
		return Role::create(model);
	}
	else if(type == "Item")
	{
		return Item::create(model);
	}
	else if(type == "Prop")
	{
		return Prop::create(model);
	}
	else if(type == "Equip")
	{
		return Equip::create(model);
	}
	else if(type == "Special")
	{
		return Special::create(model);
	}
	else if(type == "Gem")
	{
		return Gem::create(model);
	}
	else if(type == "Door")
	{
		return Door::create(model);
	}
	else if(type == "Switch")
	{
		return Switch::create(model);
	}
	else if(type == "Effect")
	{
		return Effect::create(model);
	}
	else if(type == "Spar")
	{
		return Spar::create(model);
	}
	else
	{
		return nullptr;
	}
}
