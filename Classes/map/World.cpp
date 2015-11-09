#include "World.h"
#include "utils/AStar.h"
#include "Detect.h"
#include "model/Model.h"
#include "utils/PlotScript.h"

World::World()
{

}

World::World(ValueMap model)
{
	_model = model;
}

World *World::create()
{
	World *ret = new World();
	if (ret && ret->init())
	{
	ret->autorelease();
	return ret;
	}
	else
	{
	CC_SAFE_DELETE(ret);
	return nullptr;
	}
}

World* World::create(ValueMap model)
{
	World *ret = new World(model);
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

void World::initModel()
{
	//if(DATA_MODE == 1)
	//{
	//	// 初始化数据
	//	rapidjson::Document &datas = Model::shareModel()->towerDatas;
	//	auto &data = Model::GetDataFromCol1(datas,_model["id"].asInt());
	//	Model::JsonAddModel(data,_model);
	//}
	//else if(DATA_MODE == 2)
	//{
	//	auto model = Model::shareModel()->towerCsv->getModelById(_model["id"].asInt());
	//	Model::modelAddModel(model,_model);
	//}
	//stringToValueMap(_model["towers"].asString(),_towerUrls);

	//_towersNum = (int)_towerUrls.size();
	//_towers = Map<int, Tower*>(_towerUrls.size());

	if(_model["name"].isNull())
	{
		_model["name"] = "world";
	}
	setName(_model["name"].asString());

	auto normalStr = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "treasures","normals");
	auto goodsStr = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "treasures","goods");
	auto raresStr = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "treasures","rares");
	auto fixStr = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "treasures","fixs");
	ValueVector normals;
	stringToValueVector(std::string(normalStr),"|",normals);
	ValueVector goods;
	stringToValueVector(std::string(goodsStr),"|",goods);
	ValueVector rares;
	stringToValueVector(std::string(raresStr),"|",rares);
	ValueVector fixs;
	stringToValueVector(std::string(fixStr),"|",fixs);

	_treasures["normals"] = std::vector<std::string>();
	_treasures["goods"] = std::vector<std::string>();
	_treasures["rares"] = std::vector<std::string>();
	_treasures["fixs"] = std::vector<std::string>();
	for(auto cell:normals)
	{
		_treasures["normals"].push_back(cell.asString());
	}
	for(auto cell:goods)
	{
		_treasures["goods"].push_back(cell.asString());
	}
	for(auto cell:rares)
	{
		_treasures["rares"].push_back(cell.asString());
	}
	for(auto cell:fixs)
	{
		_treasures["fixs"].push_back(cell.asString());
	}

	shake = RepeatForever::create(Shake::create(1000,2,2));

	/*shake = RepeatForever::create(
		Sequence::create(
		MoveBy::create(0.05f, Vec2(10,10)),
		MoveBy::create(0.05f, Vec2(-10,-10)),
		nullptr));*/
	shake->retain();
}

void World::removeAllCells()
{
	for(auto pair:_towers)
	{
		pair.second->removeAllCells();
	}
}

void World::loadModel(ValueMap &map)
{
	auto towersModel = map["towers"].asValueMap();
	for(auto pair:towersModel)
	{
		auto towerId = cocos2d::Value(pair.first).asInt();
		auto tower = getTower(towerId);
		auto towerModel = pair.second.asValueMap();
		auto floorsModel = towerModel["floors"].asValueMap();
		for(auto pair:floorsModel)
		{
			auto floorId = cocos2d::Value(pair.first).asInt();
			Detect::shareDetect()->getFloor(floorId,towerId);
		}
	}
	Detect::shareDetect()->removeAllCell();

	for(auto iter = _towers.begin(); iter != _towers.end(); )
	{
		auto id = (*iter).first;
		auto tower = (*iter).second;
		if(towersModel.find(cocos2d::Value(id).asString()) != towersModel.end())
		{
			tower->loadModel(towersModel[cocos2d::Value(id).asString()].asValueMap());
			++iter;
		}
		else
		{
			if(tower == Detect::shareDetect()->getCurTower())
			{
				Detect::shareDetect()->resetCurTower();
			}
			//tower->removeAllCells();
			tower->removeFromParentAndCleanup(true);
			_towers.erase(iter++);
		}
	}

	if(!map["treasures"].isNull())
	{
		_treasures["normals"].clear();
		_treasures["goods"].clear();
		_treasures["rares"].clear();
		auto treasures = map["treasures"].asValueMap();
		auto normals = treasures["normals"].asValueVector();
		auto goods = treasures["goods"].asValueVector();
		auto rares = treasures["rares"].asValueVector();
		for(auto cell:normals)
		{
			_treasures["normals"].push_back(cell.asString());
		}
		for(auto cell:goods)
		{
			_treasures["goods"].push_back(cell.asString());
		}
		for(auto cell:rares)
		{
			_treasures["rares"].push_back(cell.asString());
		}
	}
}

void World::refreshMap()
{
	for(auto pair : _towers)
	{
		pair.second->refreshMap();
	}
}

ValueMap World::saveModel()
{
	ValueMap model;
	ValueMap towers;
	for(auto pair : _towers)
	{
		towers[cocos2d::Value(pair.first).asString()] = pair.second->saveModel();
	}
	model["towers"] = towers;

	ValueMap treasures;
	ValueVector normals;
	ValueVector goods;
	ValueVector rares;
	for(auto cell:_treasures["normals"])
	{
		normals.push_back(cocos2d::Value(cell));
	}
	for(auto cell:_treasures["goods"])
	{
		goods.push_back(cocos2d::Value(cell));
	}
	for(auto cell:_treasures["rares"])
	{
		rares.push_back(cocos2d::Value(cell));
	}
	treasures["normals"] = normals;
	treasures["goods"] = goods;
	treasures["rares"] = rares;
	
	model["treasures"] = treasures;

	return model;
}

bool World::init()
{
	Layer::init();
	bool bRet = false;
	do{
		initModel();
		//_listener = EventListenerCustom::create(getName(), CC_CALLBACK_1(World::onTrigger, this));
		_listener = EventListenerCustom::create(getName(), [=](EventCustom *event){onTrigger(event);});
		CC_SAFE_RETAIN(_listener);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
		bRet = true;
	} while (0);
	return bRet;
}

World::~World()
{
	_eventDispatcher->removeEventListener(_listener);
	CC_SAFE_RELEASE(_listener);
};

void World::onEnter()
{
	Layer::onEnter();
}

void World::onExit()
{
	Layer::onExit();
}

void World::removeTreasure(std::string name)
{
	for(auto pair:_treasures)
	{
		for(auto treasure:pair.second)
		{
			if(treasure == name)
			{
				_treasures[pair.first].erase(std::find(_treasures[pair.first].begin(),_treasures[pair.first].end(),treasure));
				return;
			}
		}
	}
}

std::map<std::string, std::vector<std::string>> World::getTreasures()
{
	return _treasures;
}

std::string World::getRandTreasure(std::string type)
{
	int size;
	int index;
	std::vector<std::string> groups;
	if(type == "normal")
		groups = _treasures["normals"];
	else if(type == "good")
		groups = _treasures["goods"];
	else if(type == "rare")
		groups = _treasures["rares"];
	size = groups.size();
	if(size == 0) return "";
	index = CCRANDOM_0_1()*(size-1);
	return groups.at(index);
}

Tower* World::getTower(int towerId)
{
	Tower* tower = _towers.at(towerId);
	// 塔缓存初始化
	if(tower == nullptr)
	{
		ValueMap model;
		model["id"] = towerId;
		model["name"] = StringUtils::format("tower%d", towerId);
		tower = Tower::create(model);
		_towers.insert(towerId, tower);
		tower->setWorld(this);
	}
	return tower;
}

std::map<int, bool> World::getUnlockTowerId()
{
	std::map<int, bool> Mret;
	for(auto pair:_towerUrls)
	{
		int id = cocos2d::Value(pair.first).asInt();
		if(_towers.find(id) != _towers.end())
		{
			Mret.insert(std::make_pair(id,true));
		}
		else
		{
			Mret.insert(std::make_pair(id,false));
		}		
	}
	return Mret;
}

const std::string World::getNameId()
{
	return _model["name"].asString();
}

bool World::onTrigger(EventCustom *event)
{
	//_curTower->onTrigger(event);
	/*auto data = static_cast<ValueMap*>(event->getUserData());
	if((*data)["type"].asString() == "remove")
	{
	auto cell = dynamic_cast<Cell*>(getCellByName((*data)["senderName"].asString()));
	removeCell(cell);
	}*/
	return false;
}