#include "Detect.h"
#include "mutual/ChatManager.h"
#include "cell/CellFactory.h"
#include "cell/effect/Effect.h"
#include "utils/Altar.h"
#include "utils/PlotScript.h"
#include "ui/PopPanel.h"
#include "ui/PaperPanel.h"
#include "ui/EndPanel.h"
#include "SimpleAudioEngine.h"
#include "User.h"
#include "UI/SharePanel.h"

using namespace CocosDenshion;

static Detect* ret = nullptr;

Detect::Detect()
{
	initModel();
}

void Detect::initModel()
{
	_player = nullptr;
	_world = nullptr;
	_curTower = nullptr;
	_curFloor = nullptr;
	_controlled = true;
	_joyStick = nullptr;
	lookActorOpen = false;
	_curBGmusic = "";
	viewHide = cocos2d::Value(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", "viewHide")).asInt();

	_soundOn = true;
	_effectOn = true;
	setName("detect");
	//_listener = EventListenerCustom::create("detect", CC_CALLBACK_1(Detect::onTrigger, this));
	_listener = EventListenerCustom::create("detect", [=](EventCustom *event){onTrigger(event);});
	_eventDispatcher->addEventListenerWithFixedPriority(_listener, 1);

	setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	_controlCondition["1"] = true;
	_controlCondition["2"] = true;
	setControlled(1, true);

	_worldDark = LayerColor::create(Color4B::BLACK);
	_worldDark->retain();
	_worldDark->setOpacity(0);

	_user = User::create();
}

void Detect::loadModel(ValueMap &map)
{
	_player = nullptr;
	cellCount = map["cellCount"].asInt();
	lookActorOpen = bool(map["lookActorOpen"].asInt());
	auto worldV = map["world"];
	if(!worldV.isNull())
	{
		_world->loadModel(worldV.asValueMap());
	}

	auto curFloorV = map["curFloor"];
	auto curTowerV = map["curTower"];
	if(!curFloorV.isNull() && !curTowerV.isNull())
	{
		setCurFloor(curFloorV.asInt(), curTowerV.asInt());
	}

	auto player = dynamic_cast<Player*>(Detect::shareDetect()->getCellByName(map["player"].asString()));
	setPlayer(player);

	auto waitingRoomV = map["waitingRoom"];
	if(!waitingRoomV.isNull())
	{
		_waitingRoom.clear();
		for(auto pair:waitingRoomV.asValueMap())
		{
			addCellToWaitingRoom(CellFactory::getInstance()->createCell(pair.second.asValueMap()));
		}
	}
	auto chatManagerV = map["chatManager"];
	if(!chatManagerV.isNull())
	{
		ChatManager::getInstance()->loadModel(chatManagerV.asValueMap());
	}
	auto conditionV = map["conditions"];
	if(!conditionV.isNull())
	{
		conditions.clear();
		for(auto pair:conditionV.asValueMap())
		{
			conditions[pair.first] = LuaValue::intValue(pair.second.asInt());
		}
	}
	auto ctrlConditionV = map["controlCondition"];
	if(!ctrlConditionV.isNull())
	{
		for(auto pair:ctrlConditionV.asValueMap())
		{
			auto tag = cocos2d::Value(pair.first).asInt();
			if(tag == 2) 
				setControlled(2,true);
			else
				setControlled(tag, pair.second.asBool());
		}
	}

	auto curBGmusicV = map["curBGmusic"];
	if(!curBGmusicV.isNull())
	{
		auto bgMusic = curBGmusicV.asString();
		if(_curBGmusic != bgMusic)
		{
			if(bgMusic == "")
			{
				EventHelper::getInstance()->dispatchByStr("target:detect|type:stopBG","");
			}
			else
			{
				EventHelper::getInstance()->dispatchByStr("target:detect|type:playBG",StringUtils::format("name:%s|loop:1",bgMusic.c_str()));
			}
		}
	}
}

ValueMap Detect::saveModel()
{
	ValueMap model;
	model["world"] = getWorld()->saveModel();
	model["curTower"] = getCurTower()->getId();
	model["curFloor"] = getCurFloor()->getId();
	model["player"] = getPlayer()->getName();
	ValueMap waitingRoom;
	for(auto pair:_waitingRoom)
	{
		waitingRoom[pair.first] = pair.second->saveModel();
	}
	model["waitingRoom"] = waitingRoom;
	// 对话
	model["chatManager"] = ChatManager::getInstance()->saveModel();
	// 剧情
	ValueMap conditionMap;
	for(auto pair:conditions)
	{
		conditionMap[pair.first] = conditions[pair.first].intValue();
	}
	model["conditions"] = conditionMap;

	model["controlCondition"] = _controlCondition;

	model["cellCount"] = cellCount;

	model["lookActorOpen"] = int(lookActorOpen);

	// 背景音乐
	model["curBGmusic"] = _curBGmusic;
	return model;
}

void Detect::setJoyStick(JoyStickPanel* joyStick)
{
	_joyStick = joyStick;
	_joyStick->onDirection = CC_CALLBACK_1(Detect::onDirection,this);//角度变化更新（onDirection(JoystickEnum enums)）
	_joyStick->onHit = CC_CALLBACK_2(Detect::onHit,this);
	_joyStick->onRun();//启动
}

Detect::~Detect()
{
	_eventDispatcher->removeEventListener(_listener);
}

Detect* Detect::shareDetect()
{
	if(ret == nullptr)
	{
		ret = new Detect();
		ret->autorelease();
		ret->retain();
	}
	return ret;
}

void Detect::reset()
{
	if(this->getParent() != nullptr)
	{
		this->removeFromParent();
	}
	_curTower = nullptr;
	_player = nullptr;
	_curFloor = nullptr;
	_joyStick = nullptr;
	lookActorOpen = false;
	_controlled = true;
	_curBGmusic = "";
	viewHide = cocos2d::Value(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", "viewHide")).asInt();

	removeAllCell();
	_cells.clear();
	_waitingRoom.clear();
	//CC_SAFE_RELEASE(ret);
	//ret == nullptr;
	for(auto pair:_controlCondition)
	{
		_controlCondition[pair.first] = true;
	}
	conditions.clear();
}

void Detect::onEnter()
{
	Layer::onEnter();
}

void Detect::setControlled(int tag,bool b)
{
	if(tag == 3)
	{
		if(b == false)
		{
			EventHelper::getInstance()->dispatchByStr("target:detect|type:setBGVolume","num:0.2");
			EventHelper::getInstance()->dispatchByStr("target:hud|type:plotBlock","action:in");
		}
		else
		{
			EventHelper::getInstance()->dispatchByStr("target:detect|type:setBGVolume","num:1");
			EventHelper::getInstance()->dispatchByStr("target:hud|type:plotBlock","action:out");
		}
	}
	_controlCondition[cocos2d::Value(tag).asString()] = b;
	if(b == false)
	{
		setTouchEnabled(b);
		setKeyboardEnabled(b);
		_controlled = b;
		if(tag != 3)
		{
			EventHelper::getInstance()->dispatchByStr("target:player|type:stop","");
		}
	}
	else
	{
		auto bo = true;
		for(auto pair:_controlCondition)
		{
			if(pair.second.asBool() == false)
			{
				bo = false;
			}
		}
		if(bo == true)
		{
			setTouchEnabled(bo);
			setKeyboardEnabled(bo);
			_controlled = bo;
		}
	}
}

void Detect::addCell(Cell* cell, Point site, int layerId, int floorId, int towerId)
{
	getTower(towerId)->getFloor(floorId)->addCell(cell,site,layerId);
}

void Detect::removeCell(Cell* cell)
{
	cell->removeFromFloor();
}

void Detect::refreshCells()
{
	Model::shareModel()->refresh();
	for(auto cell:_cells)
	{
		cell.second->refresh();
	}
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}

void Detect::refreshPlayer()
{
	auto player = Detect::shareDetect()->getPlayer();
	auto hp = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","hp")).asInt();
	auto str = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","str")).asInt();
	auto def = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","def")).asInt();
	auto xp = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","xp")).asInt();
	auto gold = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","gold")).asInt();
	auto key1 = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","key1")).asInt();
	auto key2 = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","key2")).asInt();
	auto key3 = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "playerDebug","key3")).asInt();

	player->addHp(hp);
	player->addStr(str);
	player->addDef(def);
	player->addXp(xp);
	player->addGold(gold);

	if(key1 > 0)
	{
		player->addPropByType(2002001,key1);
	}
	else if(key1 < 0)
	{
		player->removePropByType(2002001,-key1);
	}

	if(key2 > 0)
	{
		player->addPropByType(2002002,key2);
	}
	else if(key2 < 0)
	{
		player->removePropByType(2002002,-key2);
	}

	if(key3 > 0)
	{
		player->addPropByType(2002003,key3);
	}
	else if(key3 < 0)
	{
		player->removePropByType(2002003,-key3);
	}
}

bool Detect::detachCell(Cell *cell)
{
	if(_cells.find(cell->getName()) == _cells.end()) return false;
	_cells.erase(cell->getName());
	return true;
}

bool Detect::attachCell(Cell* cell)
{
	if(_cells.find(cell->getName()) != _cells.end()) return false;
	_cells.insert(cell->getName(), cell);
	return true;
}

void Detect::removeAllCell()
{
	Vector<Cell*> cells;
	for(auto pair:_cells)
	{
		cells.pushBack(pair.second);
	}
	for(auto cell:cells)
	{
		cell->removeFromFloor();
	}
}

void Detect::addCellByType(int typeId, Point site, int layerId, int foorId, int towerId)
{
	ValueMap model;
	model["typeId"] = typeId;
	std::string name;
	if(model["name"].isNull())
		name = StringUtils::format("Cell_%02d%02d%02d%02d",towerId,foorId,(int)site.x,(int)site.y);
	else
		name = model["name"].asString();
	model["name"] = name;
	auto cell = CellFactory::getInstance()->createCell(model);
  	addCell(cell,site,layerId,foorId,towerId);
}

Cell* Detect::getCellByName(std::string name)
{
	return _cells.at(name);
}

Floor* Detect::getFloor(int id, int towerId)
{
	if(towerId == -1)
	{
		return _curTower->getFloor(id);
	}
	else
	{
		return getTower(towerId)->getFloor(id);
	}
}

Tower* Detect::getTower(int id)
{
	return getWorld()->getTower(id);
}

Floor* Detect::getCurFloor()
{
	return _curFloor;
}

void Detect::resetCurFloor()
{
	_curFloor = nullptr;
}

void Detect::setCurFloor(int floorId, int towerId, float duration)
{
	// 当前楼层返回
	if(_curFloor != nullptr && floorId == _curFloor->getId() && towerId == _curFloor->getTower()->getId())
		return;

	auto oldTower = _curTower;
	auto oldFloor = _curFloor;

	if(towerId == -1)
		towerId = getCurTower()->getId();
	// 当前显示的楼层切换
	Tower* newTower = getTower(towerId);
	if(_world->getChildByTag(towerId) == nullptr)
	{
		_world->addChild(newTower,towerId,towerId);
		newTower->setVisible(false);
	}

	Floor* newFloor = getFloor(floorId, towerId);
	newFloor->buildMap();
	if(newTower->getChildByTag(floorId) == nullptr)
	{
		newTower->addChild(newFloor,floorId,floorId);
		newFloor->setVisible(false);
	}
	setControlled(4,false);

	_curFloor = newFloor;
	_curTower = newTower;

	_worldDark->removeFromParent();
	_world->addChild(_worldDark,100);
	_worldDark->runAction(Sequence::create(FadeTo::create(duration,180),CallFunc::create([=](){
		/*for(auto node:_world->getChildren())
		{
		if(node != _worldDark)
		{

		node->setVisible(false);
		}
		}*/
		if(oldTower != nullptr)
		{
			oldTower->setVisible(false);
		}
		if(oldFloor != nullptr)
		{
			oldFloor->setVisible(false);
		}
		newTower->setVisible(true);
		newFloor->setVisible(true);
		newTower->setZOrder(-1);
		newFloor->setZOrder(-1);


#if DETECT_ACTOR == 1
		_curFloor->updateFight();
#endif
		EventHelper::getInstance()->dispatchByStr("target:hud|senderName:Detect|type:setCurFloor", StringUtils::format("floorId:%d|towerName:%s",floorId,_curTower->getNickName().c_str()));
	}),FadeTo::create(duration,0),CallFuncN::create([this](Node *node){setControlled(4,true);node->removeFromParent();}),NULL));
}

void Detect::resetCurTower()
{
	_curTower = nullptr;
}

Tower* Detect::getCurTower()
{
	return _curTower;
}

Player* Detect::getPlayer()
{
	return _player;
}

void Detect::setPlayer(Player* player)
{
	_player = player;
	if(_player != nullptr) _player->updateToHud();
}

void Detect::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	Actor::Direction guide = Actor::Direction::STOP;
	if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		guide = Actor::Direction::LEFT;
	}
	else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		guide = Actor::Direction::RIGHT;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
	{
		guide = Actor::Direction::UP;
	}
	else if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
	{
		guide = Actor::Direction::DOWN;
	}
	else if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:touch","");
	}
	if(guide != Actor::Direction::STOP)
	{
		EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:setGuide",StringUtils::format("guide:%d|value:%d",(int)guide,1));
	}
}

void Detect::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	Actor::Direction guide = Actor::Direction::STOP;
	if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		guide = Actor::Direction::LEFT;
	}
	else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		guide = Actor::Direction::RIGHT;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
	{
		guide = Actor::Direction::UP;
	}
	else if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
	{
		guide = Actor::Direction::DOWN;
	}
	if(guide != Actor::Direction::STOP)
	{
		if(getPlayer())
		{
			EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:setGuide",StringUtils::format("guide:%d|value:%d",(int)guide,0));
		}		
	}
}


void Detect::onDirection(JoyStickPanel::JoystickEnum enums)
{
	if(_controlled == false) return;
	if(getPlayer())
	{
		EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:setGuide",StringUtils::format("guide:%d|value:%d",Actor::Direction::UP,0));
		EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:setGuide",StringUtils::format("guide:%d|value:%d",Actor::Direction::LEFT,0));
		EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:setGuide",StringUtils::format("guide:%d|value:%d",Actor::Direction::RIGHT,0));
		EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:setGuide",StringUtils::format("guide:%d|value:%d",Actor::Direction::DOWN,0));
	}
	Actor::Direction guide = Actor::Direction::STOP;
	switch (enums)
	{
	case JoyStickPanel::DEFAULT:
		break;
	case JoyStickPanel::D_UP:
		guide = Actor::Direction::UP;
		break;
	case JoyStickPanel::D_DOWN:
		guide = Actor::Direction::DOWN;
		break;
	case JoyStickPanel::D_LEFT:
		guide = Actor::Direction::LEFT;
		break;
	case JoyStickPanel::D_RIGHT:
		guide = Actor::Direction::RIGHT;
		break;
	default:
		break;
	}
	if(guide != Actor::Direction::STOP)
	{
		if(getPlayer())
		{
			EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:setGuide",StringUtils::format("guide:%d|value:%d",(int)guide,1));
		}
	}
}

void Detect::onHit(cocos2d::Ref *ref, ui::Widget::TouchEventType touchType)
{
	if (touchType == ui::Widget::TouchEventType::ENDED) 
	{
		if(ChatManager::getInstance()->on == true)
		{
			ChatManager::getInstance()->nextChat();
			return;
		}
		if(_controlled == false) return;
		if(_player != nullptr)
			EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:touch","");
	}

}

bool Detect::onTouchBegan(Touch *touch, Event *unused_event)
{
	Point touchLocation = getWorld()->convertTouchToNodeSpace(touch);
	auto site = POSTOSITE(touchLocation);
	if(getPlayer())
	{
		EventHelper::getInstance()->dispatchByStr("target:"+getPlayer()->getName()+"|type:guideTo",StringUtils::format("site:{%f,%f}",site.x,site.y));
	}
	return false;
}

void Detect::onTouchEnded(Touch *touch, Event *unused_event)
{

}

bool Detect::onTrigger(EventCustom *event)
{
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	if((*data)["type"].asString() == "setControlled")
	{
		int tag = effect["tag"].asInt();
		int value = effect["value"].asInt();
		setControlled(tag, (value == 1?true:false));
	}
	else if((*data)["type"].asString() == "remove")
	{
		auto name = effect["name"].asString();
		getCellByName(name)->removeFromFloor();
	}
	else if((*data)["type"].asString() == "removeToWaitingRoom")
	{
		auto name = effect["name"].asString();
		auto cell = getCellByName(name);
		if(cell != nullptr)
		{
			_waitingRoom.insert(name,cell);
			cell->removeFromFloor();
		}
	}
	else if((*data)["type"].asString() == "addByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto site = PointFromString(effect["site"].asString());
		int layerId = 0;
		if(!effect["layerId"].isNull())
			layerId = effect["layerId"].asInt();
		int floorId = getCurFloor()->getId();
		if(!effect["floorId"].isNull())
			floorId = effect["floorId"].asInt();
		int towerId = getCurTower()->getId();
		if(!effect["towerId"].isNull())
			towerId = effect["towerId"].asInt();
		addCellByType(typeId,site,layerId,floorId,towerId);
	}
	else if((*data)["type"].asString() == "add")
	{
		ValueMap map;
		Model::modelAddModel(effect, map);
		/*map["typeId"] = effect["typeId"].asInt();
		map["name"] = effect["name"].asString();*/
		auto cell = CellFactory::getInstance()->createCell(map);
		auto site = PointFromString(effect["site"].asString());
		int layerId = 0;
		if(!effect["layerId"].isNull())
			layerId = effect["layerId"].asInt();
		int floorId = getCurFloor()->getId();
		if(!effect["floorId"].isNull())
			floorId = effect["floorId"].asInt();
		int towerId = getCurTower()->getId();
		if(!effect["towerId"].isNull())
			towerId = effect["towerId"].asInt();
		addCell(cell,site,layerId,floorId,towerId);
	}
	else if((*data)["type"].asString() == "addEffect")
	{
		auto typeId = effect["typeId"].asInt();
		auto site = PointFromString(effect["site"].asString());
		int layerId = 4;
		if(!effect["layerId"].isNull())
			layerId = effect["layerId"].asInt();
		int floorId = getCurFloor()->getId();
		if(!effect["floorId"].isNull())
			floorId = effect["floorId"].asInt();
		int towerId = getCurTower()->getId();
		if(!effect["towerId"].isNull())
			towerId = effect["towerId"].asInt();

		ValueMap model;
		model["typeId"] = typeId;
		auto cell = CellFactory::getInstance()->createCell(model);
		auto effect = dynamic_cast<Effect*>(cell);
		addCell(effect,site,layerId,floorId,towerId);
	}
	else if((*data)["type"].asString() == "addFromWaitingRoom")
	{
		auto name = effect["name"].asString();
		auto site = PointFromString(effect["site"].asString());
		int layerId = 3;
		if(!effect["layerId"].isNull())
			layerId = effect["layerId"].asInt();
		int floorId = getCurFloor()->getId();
		if(!effect["floorId"].isNull())
			floorId = effect["floorId"].asInt();

		Cell* cell = _waitingRoom.at(name);
		if(cell != nullptr)
		{
			addCell(cell,site,layerId,floorId);
			removeCellFromWaitingRoom(cell);
		}
		else
		{
			cell = getCellByName(name);
			cell->retain();
			removeCell(cell);
			addCell(cell,site,layerId,floorId);
			cell->release();
		}
	}
	else if((*data)["type"].asString() == "addCellToWaitingRoom")
	{
		ValueMap map;
		map["typeId"] = effect["typeId"].asInt();
		map["name"] = effect["name"].asString();
		map["chatId"] = effect["chatId"].asInt();
		auto cell = CellFactory::getInstance()->createCell(map);
		addCellToWaitingRoom(cell);
	}
	else if((*data)["type"].asString() == "removeCellFromWaitingRoom")
	{

	}
	else if((*data)["type"].asString() == "setCellModelByName")
	{
		auto name = effect["name"].asString();
		auto modelName = effect["modelName"].asString();
		auto value = effect["value"];
		getCellByName(name)->setModelByName(modelName,value);
	}
	else if((*data)["type"].asString() == "towerStartShake")
	{
		_curTower->runAction(_curTower->shake);
	}
	else if((*data)["type"].asString() == "towerEndShake")
	{
		_curTower->stopAction(_curTower->shake);
	}
	else if((*data)["type"].asString() == "cacheFloor")
	{
		auto floorId = effect["floorId"].asInt();
		auto tower = _curTower;
		if(!effect["towerId"].isNull())
		{
			tower = getTower(effect["towerId"].asInt());
		}
		tower->getFloor(floorId);
	}
	//else if((*data)["type"].asString() == "buildFloor")
	//{
	//	auto floorId = effect["floorId"].asInt();
	//	auto tower = _curTower;
	//	if(!effect["towerId"].isNull())
	//	{
	//		tower = getTower(effect["towerId"].asInt());
	//	}
	//	tower->buildFloor(floorId);
	//}
	else if((*data)["type"].asString() == "explore")
	{
		std::string filename = "Particles/ExplodingRing.plist";
		auto emitter = ParticleSystemQuad::create(filename);
		emitter->retain();
		_curTower->addChild(emitter, 10);

		Point site(0,0);
		Point position(0,0);

		if(!effect["toCellName"].isNull())
		{
			auto name = effect["toCellName"].asString();
			auto cell = Detect::shareDetect()->getCellByName(name);
			position = cell->getPosition();
		}
		if(!effect["site"].isNull())
		{
			site = PointFromString(effect["site"].asString());
			position = Vec2(site.x*TILE_WIDTH+TILE_HALF_WIDTH,site.y*TILE_HEIGHT+TILE_HALF_HEIGHT);
		}
		
		if (emitter != nullptr)
		{
			emitter->setPosition(position);
		}

		auto shake = Shake::create(1.0f,2,2);
		_curTower->runAction(shake);
	}
	else if((*data)["type"].asString() == "setCurFloor")
	{
		auto floorId = effect["floorId"].asInt();

		int towerId = -1;
		if(!effect["towerId"].isNull())
		{
			towerId = effect["towerId"].asInt();
		}
		auto duration = effect["duration"].asFloat();
		setCurFloor(floorId, towerId, duration);
	}
	else if((*data)["type"].asString() == "setPlayer")
	{
		auto name = effect["name"].asString();

		auto cell = getCellByName(name);
		auto player = dynamic_cast<Player*>(cell);
		if(player != nullptr)
			setPlayer(player);
	}
	else if((*data)["type"].asString() == "addAltar")
	{
		auto altar = Altar::create();
		auto site = PointFromString(effect["site"].asString());
		int layerId = 0;
		if(!effect["layerId"].isNull())
			layerId = effect["layerId"].asInt();
		int floorId = getCurFloor()->getId();
		if(!effect["floorId"].isNull())
			floorId = effect["floorId"].asInt();
		int towerId = getCurTower()->getId();
		if(!effect["towerId"].isNull())
			towerId = effect["towerId"].asInt();
		auto floor = getFloor(floorId,towerId);
		floor->altar = altar;
		floor->addChild(altar);
	}
	else if((*data)["type"].asString() == "disappear")
	{
		std::string name;
		if(!effect["name"].isNull())
			name = effect["name"].asString();

		auto cell = getCellByName(name);
		cell->disappear();
	}
	else if((*data)["type"].asString() == "note")
	{
		float delay = 1.0f;
		if(!effect["delay"].isNull())
			delay = effect["delay"].asFloat();

		bool single = false;
		if(!effect["single"].isNull())
			single = effect["single"].asBool();

		bool center = false;
		if(!effect["center"].isNull())
			center = effect["center"].asBool();

		std::string content;
		if(!effect["content"].isNull())
			content = effect["content"].asString();

		stringReplace(content,"^","|");

		PopPanel::getInstance()->note("cannt","type:Text---text:"+ content, delay, single, center);
	}
	else if((*data)["type"].asString() == "diary")
	{
		auto type = effect["type"].asString();
		auto content = effect["content"].asString();
		auto diary = getPlayer()->getDiary();
		if(type == "tip")
		{
			diary->addTip(content);
		}
		else if(type == "task")
		{
			diary->addTask(content);
		}
		else if(type == "map")
		{
			diary->addMap(content);
		}
		diary->show(type,content);
		PopPanel::getInstance()->addPanel(diary,1);
	}
	else if((*data)["type"].asString() == "paper")
	{
		std::string text;
		if(!effect["textNum"].isNull())
		{
			auto paper = PaperPanel::create("paper");
			paper->initWithTextNum(effect["textNum"].asInt());
			PopPanel::getInstance()->addPanel(paper,1);
		}
		else
		{
			text = effect["text"].asString();
			auto paper = PaperPanel::create("paper");
			paper->initWithText(text);
			PopPanel::getInstance()->addPanel(paper,1);
		}
	}
	else if((*data)["type"].asString() == "stopWeather")
	{
		auto floorId = effect["floorId"].asInt();
		auto towerId = effect["towerId"].asInt();
		auto floor = getFloor(floorId,towerId);
		floor->stopWeather();
	}
	else if((*data)["type"].asString() == "setEnd")
	{
		auto endId = effect["endId"].asInt();
		EndPanel* end = EndPanel::create("end");
		end->initWithNum(endId);
		PopPanel::getInstance()->addPanel(end,1);
	}
	else if((*data)["type"].asString() == "playBG")
	{
		if(_soundOn == false)
			return false;
		auto name = effect["name"].asString();
		_curBGmusic = name;
		std::string loopStr;
		if(!effect["loop"].isNull())
			loopStr = effect["loop"].asString();
		auto loop = cocos2d::Value(loopStr).asInt();
		auto path = getMscPathByName(name);
		SimpleAudioEngine::getInstance()->playBackgroundMusic(path.c_str(),bool(loop));
	}
	else if((*data)["type"].asString() == "playEffect")
	{
		if(_soundOn == false)
			return false;
		auto name = effect["name"].asString();
		int loop = 0;
		if(!effect["loop"].isNull())
			loop = effect["loop"].asInt();
		float pitch = 1.0f;
		if(!effect["pitch"].isNull())
			pitch = effect["pitch"].asFloat();
		float pan = 0.0f;
		if(!effect["pan"].isNull())
			pan	= effect["pan"].asFloat();
		float gain = 1.0f;
		if(!effect["gain"].isNull())
			gain = effect["gain"].asFloat();
		
		SimpleAudioEngine::getInstance()->playEffect(getMscPathByName(name).c_str(),bool(loop),pitch,pan,gain);
	}
	else if((*data)["type"].asString() == "stopAllEffect")
	{
		SimpleAudioEngine::getInstance()->stopAllEffects();
	}
	else if((*data)["type"].asString() == "stopBG")
	{
		auto name = effect["name"].asString();
		int bRelease = 0;
		if(!effect["bRelease"].isNull())
			bRelease = effect["bRelease"].asInt();
		_curBGmusic = "";
		SimpleAudioEngine::getInstance()->stopBackgroundMusic(bool(bRelease));
	}
	else if((*data)["type"].asString() == "changeBG")
	{
		auto name = effect["name"].asString();
		int bRelease = 0;
		if(!effect["bRelease"].isNull())
			bRelease = effect["bRelease"].asInt();
		//auto modifyWidth = ActionTween::create(2, "width", 200, 300);
		//target->runAction(modifyWidth);
		//SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.1);
		SimpleAudioEngine::getInstance()->stopBackgroundMusic(bool(bRelease));
	}
	else if((*data)["type"].asString() == "setBGVolume")
	{
		float num = 1;
		if(!effect["num"].isNull())
			num = effect["num"].asFloat();
		//auto modifyWidth = ActionTween::create(2, "width", 200, 300);
		//target->runAction(modifyWidth);
		//SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.1);
		SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(num);
	}
	else if((*data)["type"].asString() == "stopEffect")
	{
		auto name = effect["name"].asString();
		int bRelease = 0;
		if(!effect["pitch"].isNull())
			bRelease = effect["loop"].asInt();
		SimpleAudioEngine::getInstance()->stopEffect(bool(bRelease));
	}
	else if((*data)["type"].asString() == "shareRewardOpen")
	{
		Detect::shareDetect()->getUser()->setShareRewardOpen(true);
	}
	else if((*data)["type"].asString() == "share")
	{
		auto confirmText = effect["confirmText"].asString();
		auto text = effect["shareText"].asString();
		PopPanel::getInstance()->confirm("saveConfirm","type:Text---text:"+ confirmText,PromptPanel::TYPE_DONE_CANCEL,[=](){
			auto panel = SharePanel::create("share");
			panel->setInfo(text);
			PopPanel::getInstance()->addPanel(panel,1);
		},nullptr,true,true);
	}
	return false;
}

std::string Detect::getMscPathByName(std::string name)
{
	auto path = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "musics",name.c_str());
	std::string newPath;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	newPath = StringUtils::format("%s.ogg",path);
#else
	newPath = StringUtils::format("%s.mp3",path);
#endif
	return newPath;
}

void Detect::convey(Cell* cell, Vec2 site, int layerId, int floorId, int towerId)
{
	conditions[cell->getName()+"_convey_"+cocos2d::Value(cell->getFloor()->getId()).asString()] = LuaValue::intValue(0);
	cell->retain();
	cell->removeFromFloor();
	getTower(towerId)->getFloor(floorId)->addCell(cell,site,layerId);
	cell->release();
	conditions[cell->getName()+"_convey_"+cocos2d::Value(floorId).asString()] = LuaValue::intValue(1);

	runAction(Sequence::create(DelayTime::create(0.4f),CallFunc::create([=](){
		/*ParticleSystemQuad* appearanceEmitter = ParticleSystemQuad::create("particles/appearance.plist");
		appearanceEmitter->retain();
		appearanceEmitter->setDuration(0.2f);
		appearanceEmitter->setLife(0.7f);
		Vec2 postion = Vec2(site.x * TILE_WIDTH + TILE_HALF_WIDTH,site.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
		appearanceEmitter->setPosition(postion);
		_curTower->addChild(appearanceEmitter ,99);*/
		auto appearance = Effect::create(3002001);
		addCell(appearance,site,layerId,floorId,towerId);
	}),nullptr));
}

const std::string Detect::getNameId()
{
	return this->getName();
}

std::string Detect::getType()
{
	return "Detect";
}

void Detect::addCellToWaitingRoom(Cell* cell)
{
	_waitingRoom.insert(cell->getName(),cell);
}

void Detect::removeCellFromWaitingRoom(Cell* cell)
{
	_waitingRoom.erase(cell->getName());
}

Cell* Detect::getCellFromWaitingRoomByName(std::string name)
{
	for(auto pair:_waitingRoom)
	{
		if(pair.first == name)
		{
			return pair.second;
		}
	}
	return nullptr;
}

void Detect::setSoundOn(bool value)
{
	if(value == false)
	{
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	}
	else
	{
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	}
	_soundOn = value;
}

void Detect::setEffectOn(bool value)
{
	if(value == false)
	{
		//SimpleAudioEngine::getInstance()->pauseAllEffects();
		
		SimpleAudioEngine::getInstance()->setEffectsVolume(0);
	}
	else
	{
		SimpleAudioEngine::getInstance()->setEffectsVolume(1);
		//SimpleAudioEngine::getInstance()->resumeAllEffects();
	}
	_effectOn = value;
}
