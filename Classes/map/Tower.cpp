#include "Tower.h"
#include "utils/AStar.h"
#include "Detect.h"
#include "model/Model.h"


Tower::Tower(int id)
{
	_id = id;
	_model["id"] = id;
}

Tower::Tower(ValueMap model)
{
	_model = model;
	_id = _model["id"].asInt();
	setName(_model["name"].asString());
}

Tower *Tower::create(int id)
{
	Tower *ret = new Tower(id);
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

Tower* Tower::create(ValueMap model)
{
	Tower *ret = new Tower(model);
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

const int Tower::getId() const
{
	return _id;
}

void Tower::initModel()
{
	if(DATA_MODE == 1)
	{
		// 初始化数据
		rapidjson::Document &datas = Model::shareModel()->towerDatas;
		auto &data = Model::GetDataFromCol1(datas,_model["id"].asInt());
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->towerCsv->getModelById(_model["id"].asInt());
		Model::modelAddModel(model,_model);
	}
	stringToValueMap(_model["floors"].asString(),_floorUrls);

	_floorsNum = (int)_floorUrls.size();
	//_floors = Map<int, Floor*>(_floorUrls.size());

	if(_model["name"].isNull())
	{
		_model["name"] = "tower";
	}
	setName(_model["name"].asString());

	shake = RepeatForever::create(Shake::create(1000,2,2));

	/*shake = RepeatForever::create(
		Sequence::create(
		MoveBy::create(0.05f, Vec2(10,10)),
		MoveBy::create(0.05f, Vec2(-10,-10)),
		nullptr));*/
	shake->retain();

	_canConvey = true;
	if(!_model["canConvey"].isNull())
	{
		_canConvey = bool(_model["canConvey"].asInt());
	}
}

std::string Tower::getNickName()
{
	return _model["nickName"].asString();
}

void Tower::loadModel(ValueMap &map)
{
	auto floorsModel = map["floors"].asValueMap();
	
	for(auto iter = _floors.begin(); iter != _floors.end(); )
	{
		auto id = (*iter).first;
		auto floor = (*iter).second;
		if(floorsModel.find(cocos2d::Value(id).asString()) != floorsModel.end())
		{
			floor->loadModel(floorsModel[cocos2d::Value(id).asString()].asValueMap());
			++iter;
		}
		else
		{
			if(floor == Detect::shareDetect()->getCurFloor())
			{
				Detect::shareDetect()->resetCurFloor();
			}
			//floor->removeAllCells();
			floor->removeFromParentAndCleanup(true);
			_floors.erase(iter++);
		}
	}
}

ValueMap Tower::saveModel()
{
	ValueMap model;
	ValueMap floors;
	for(auto pair : _floors)
	{
		floors[cocos2d::Value(pair.first).asString()] = pair.second->saveModel();
	}
	model["floors"] = floors;
	return model;
}

void Tower::refreshMap()
{
	for(auto pair : _floors)
	{
		pair.second->refreshMap();
	}
}

void Tower::removeAllCells()
{
	for(auto pair:_floors)
	{
		pair.second->removeAllCells();
	}
}

bool Tower::init()
{
	Layer::init();
	bool bRet = false;
	do{
		initModel();
		//_listener = EventListenerCustom::create(getName(), CC_CALLBACK_1(Tower::onTrigger, this));
		_listener = EventListenerCustom::create(getName(), [=](EventCustom *event){onTrigger(event);});
		CC_SAFE_RETAIN(_listener);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
		bRet = true;
	} while (0);
	return bRet;
}

Tower::~Tower()
{
	_eventDispatcher->removeEventListener(_listener);
	CC_SAFE_RELEASE(_listener);
};

void Tower::onEnter()
{
	Layer::onEnter();
}

void Tower::onExit()
{
	Layer::onExit();
}

Floor* Tower::getFloor(int floorId)
{
	Floor* floor = _floors.at(floorId);
	// 楼层缓存初始化
	if(floor == nullptr)
	{
		ValueMap model;
		model["url"] = _floorUrls[cocos2d::Value(floorId).asString()];
		model["id"] = floorId;
		model["name"] = StringUtils::format("floor%d", floorId);
		model["towerId"] = getId();
		floor = Floor::create(model);
		_floors.insert(floorId, floor);
		floor->setTower(this);
	}
	return floor;
}
//
//Floor* Tower::getFloor(int floorId)
//{
//	auto floor = buildFloor(floorId);
//	// 地图初始化
//	if(floor->mapInited == false)
//	{
//		floor->initMap();
//	}
//	return floor;
//}

std::map<int, bool> Tower::getUnlockFloorId()
{
	std::map<int, bool> Mret;
	for(auto pair:_floorUrls)
	{
		int id = cocos2d::Value(pair.first).asInt();
		if(_floors.find(id) != _floors.end())
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

const std::string Tower::getNameId()
{
	return _model["name"].asString();
}

bool Tower::onTrigger(EventCustom *event)
{
	//_curFloor->onTrigger(event);
	/*auto data = static_cast<ValueMap*>(event->getUserData());
	if((*data)["type"].asString() == "remove")
	{
	auto cell = dynamic_cast<Cell*>(getCellByName((*data)["senderName"].asString()));
	removeCell(cell);
	}*/
	return false;
}