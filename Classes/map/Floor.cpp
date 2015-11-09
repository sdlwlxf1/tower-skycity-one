#include "Floor.h"
#include "utils/AnimateByGID.h"
#include "utils/StringHandler.h"
#include "cell/CellFactory.h"
#include "base/CCNS.h"
#include "model/Model.h"
#include "Detect.h"
#include "utils/SpotLight.h"

#include "cell/actor/Actor.h"
#include "cell/item/Item.h"

USING_NS_CC;

Floor::Floor(ValueMap model)
{
	_model = model;
	_id = _model["id"].asInt();
	setName(_model["name"].asString());
}

Floor::Floor(int id)
{
	_id = id;
	setName(StringUtils::format("floor%d", id));
}

Floor *Floor::create(int id)
{
	Floor *ret = new Floor(id);	
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

Floor* Floor::create(ValueMap model)
{
	Floor *ret = new Floor(model);
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

bool Floor::init()
{
	Layer::init();
	bool bRet = false;
	do{
		
		initModel();
		//initMap();
		initOthers();
		bRet = true;
	} while (0);
	return bRet;
}


void Floor::initModel()
{
	// 初始化单元格
	for(int i = 0; i < HORIZON_TILE_NUM; i++)
	{
		for(int j = 0; j < VERTICAL_TILE_NUM; j++)
		{
			_units.insert(i + j * HORIZON_TILE_NUM, Unit::create(Vec2(i,j)));
		}
	}
	initMap();
	//_mapTemp = Layer::create();
	//addChild(_mapTemp);
	
}

void Floor::loadModel(ValueMap &map)
{
	//removeAllCells();
	initCells(map["objects"].asValueVector());
	if(!map["altar"].isNull() && altar != nullptr)
	{
		altar->loadModel(map["altar"].asValueMap());
	}
}

void Floor::refreshMap()
{
	auto mapNew = TMXTiledMap::create(_model["url"].asString());
	auto models = mapNew->getProperties();
	/*if(!models["refresh"].isNull())
	{

	}*/
	auto objects = mapNew->getObjectGroup("stage")->getObjects();
	std::map<std::string, cocos2d::Value> tempObjects;
	for(auto object:objects)
	{
		auto objectMap = object.asValueMap();
		Vec2 site;
		site.x = (int)((objectMap["x"].asFloat() + CELL_HALF_WIDTH) * 0.025);
		site.y = (int)(objectMap["y"].asFloat() * 0.025);
		if(!objectMap["site"].isNull())
		{
			site = PointFromString(objectMap["site"].asString());
		}
		std::string name;
		if(objectMap["name"].isNull())
			name = StringUtils::format("Cell_%02d%02d%02d%02d",_tower->getId(),getId(),(int)site.x,(int)site.y);
		else
			name = objectMap["name"].asString();

		if(!objectMap["remove"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(name);
			if(cell != nullptr) cell->removeFromFloor();
		}
		else if(!objectMap["add"].isNull())
		{
			if(objectMap["typeId"].isNull() && !objectMap["gid"].isNull())
			{
				auto value = _map0->getPropertiesForGID(objectMap["gid"].asInt());
				if(!value.isNull())
				{
					objectMap["typeId"] = value.asValueMap()["typeId"];
				}
				else
				{
					continue;
				}
			}
			objectMap["name"] = name;
			auto cell = CellFactory::getInstance()->createCell(objectMap);
			Detect::shareDetect()->addCell(cell,site,1,getId(),_tower->getId());
		}
		else if(!objectMap["refresh"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(name);
			if(cell != nullptr) cell->refresh();
		}
	}
	updateFight();
}

ValueMap Floor::saveModel()
{
	ValueMap model;
	ValueVector objects;
	/*for(auto pair:_units)
	{
	auto cells = pair.second->getCells();
	for(auto cell:cells)
	{
	objects.push_back(cocos2d::Value(cell->saveModel()));
	}
	}*/
	for(auto cell:_cells)
	{
		objects.push_back(cocos2d::Value(cell->saveModel()));
	}
	model["objects"] = objects;

	if(altar != nullptr)
	{
		model["altar"] = altar->saveModel();
	}

	return model;
}

void Floor::initOthers()
{
	//_listener = EventListenerCustom::create(getName(), CC_CALLBACK_1(Floor::onTrigger, this));
	_listener = EventListenerCustom::create(getName(), [=](EventCustom *event){onTrigger(event);});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);

	_mapBuilded = false;
}



void Floor::initMap()
{
	// 初始化地图
	_map0 = TMXTiledMap::create("map_0_0.tmx");
	_map0->retain();

	_map = TMXTiledMap::create(_model["url"].asString());
	CC_SAFE_RETAIN(_map);
	addChild(_map);
	
	_ground = _map->getLayer("ground");
	_under = _map->getLayer("under");
	_above = _map->getLayer("above");
	_stageGroup = _map->getObjectGroup("stage");

	_stage = Layer::create();
	//_mapTemp->addChild(_stage);

	_stage->addChild(Layer::create(),0,0);
	_stage->addChild(Layer::create(),1,1);//物品
	_stage->addChild(Layer::create(),2,2);
	_stage->addChild(Layer::create(),3,3);//角色
	_stage->addChild(Layer::create(),4,4);

	_stage->addChild(Layer::create(),10,10);//遮罩

	_map->addChild(_stage);

	_dark = Layer::create();
	_pLayer= LayerColor::create(Color4B(0,0,0,255));
	ClippingNode* pClip = ClippingNode::create();
	pClip->setInverted(true);
	pClip->setAlphaThreshold(0.5f);
	_dark->addChild(pClip, 0, 0);
	pClip->addChild(_pLayer);
	auto mask = Layer::create();
	auto shade = Layer::create();
	_dark->addChild(shade, 1, 1);
	pClip->setStencil(mask);
	addChild(_dark,1000);

	//_ground->setZOrder(0);
	//_under->setZOrder(1);
	auto zOrder = _map->getChildrenCount() - 3;
	auto zOrderValue = _stageGroup->getProperty("zOrder");
	if(!zOrderValue.isNull())
	{
		zOrder = zOrderValue.asInt() - 1;
	}
	_stage->setZOrder(zOrder);
	//_above->setZOrder(_map->getChildrenCount() - 1);	
	initCells(_stageGroup->getObjects());

	altar = nullptr;
}	

void Floor::lightUpdate(float dt)
{
	if(lightCount >= lightNum)
	{
		lightCount = 0;
		lighting();
		drawShanDian();
	}
	lightCount++;
}

void Floor::lighting()
{
	lightNum = CCRANDOM_0_1()*(10-2+1)+2;
	_lightColor->runAction(_lightAction);
}

void Floor::drawShanDian()
{
	_lightDraw->clear();
	int num = CCRANDOM_0_1()*(3-0+1)+0;
	while(num--)
	{
		Point pos1;
		Point pos2;
		pos1.x = CCRANDOM_0_1()*(480-0+1)+0;
		pos2.x = CCRANDOM_0_1()*(480-0+1)+0;

		pos1.y = CCRANDOM_0_1()*(550-500+1)+500;
		pos2.y = CCRANDOM_0_1()*(220-100+1)+100;
		//调用函数
		drawLighting(pos1.x, pos1.y, pos2.x, pos2.y, 200);
	}
	
}

void Floor::drawLighting(float x1, float y1, float x2, float y2, float displace)
{
	if(displace < 5.0f){
		//ccDrawLine(ccp(x1, y1), ccp(x2, y2));
		_lightDraw->drawSegment(ccp(x1, y1),ccp(x2, y2),0.4f,Color4F(1, 1, 1, 0.5));
	}else{
		float mid_x = (x2+x1) / 2;
		float mid_y = (y2+y1) / 2;
		mid_x += (CCRANDOM_0_1() -.5)*displace;
		mid_y += (CCRANDOM_0_1() -.5)*displace;

		drawLighting(x1, y1, mid_x, mid_y, displace/2);
		drawLighting(x2, y2, mid_x, mid_y, displace/2);
	}
}

void Floor::update(float delta)
{

}

void Floor::setWeather(std::string type)
{
	
}

void Floor::addShade(Shader* shader)
{
	if(shader == nullptr) return;
	dynamic_cast<ClippingNode*>(_dark->getChildByTag(0))->getStencil()->addChild(shader->mask);
	_dark->getChildByTag(1)->addChild(shader->shade);
}

void Floor::addShadeAt(std::string fShade, std::string fMask, Vec2 pos)
{
	auto shader = Shader::create(fShade, fMask);
	shader->autorelease();
	shader->update(pos);
	dynamic_cast<ClippingNode*>(_dark->getChildByTag(0))->getStencil()->addChild(shader->mask);
	_dark->getChildByTag(1)->addChild(shader->shade);
}

void Floor::removeShade(Shader* shader)
{
	if(shader == nullptr) return;
	dynamic_cast<ClippingNode*>(_dark->getChildByTag(0))->getStencil()->removeChild(shader->mask);
	_dark->getChildByTag(1)->removeChild(shader->shade);
}

void Floor::initCells(ValueVector &objects)
{
	// 添加对象
	for(auto object : objects)
	{
		// 编辑器数据
		auto objectMap = object.asValueMap();

		if(objectMap["typeId"].isNull() && !objectMap["gid"].isNull())
		{
			auto value = _map0->getPropertiesForGID(objectMap["gid"].asInt());
			if(!value.isNull())
			{
				objectMap["typeId"] = value.asValueMap()["typeId"];
			}
			else
			{
				continue;
			}
		}
		
		Vec2 site;
		site.x = (int)((objectMap["x"].asFloat() + CELL_HALF_WIDTH) * 0.025);
		site.y = (int)(objectMap["y"].asFloat() * 0.025);
		if(!objectMap["site"].isNull())
		{
			site = PointFromString(objectMap["site"].asString());
		}
		std::string name;
		if(objectMap["name"].isNull())
			name = StringUtils::format("Cell_%02d%02d%02d%02d",_model["towerId"].asInt(),getId(),(int)site.x,(int)site.y);
		else
			name = objectMap["name"].asString();
		objectMap["name"] = name;
		auto cell = CellFactory::getInstance()->createCell(objectMap);
		addCell(cell,site,cell->getLayer());
	}
#if DETECT_ACTOR == 1
	updateFight();
#endif
	_cellInited = true;
}

void Floor::updateMap(float dt)
{

}
//
//Cell* Floor::getCellByName(std::string name)
//{
//	for(auto pair:_units)
//	{
//		auto unit = pair.second;
//		auto cells = unit->getCells();
//		for(auto cell:cells)
//		{
//			if(cell->getName() == name)
//			{
//				return cell;
//			}
//		}
//	}
//	return nullptr;
//}

Cell* Floor::getCellByType(int type)
{
	for(auto cell:_cells)
	{
		if(cell->getTypeId() == type)
		{
			return cell;
		}
	}
	return nullptr;
}

Cell* Floor::getCellByType(int type, Vec2 site)
{
	Cell* cell = nullptr;
	auto unit = _units.at(site.x + site.y * HORIZON_TILE_NUM);
	auto cells = unit->getCells();
	for(auto cell:cells)
	{
		if(cell->getTypeId() == type)
		{
			return cell;
		}
	}
	return cell;
}

void Floor::addCell(Cell* cell, Point site, int layerId)
{
	if(site.x >= HORIZON_TILE_NUM || site.x < 0 || site.y >= VERTICAL_TILE_NUM || site.y < 0)
		return;
	if(Detect::shareDetect()->attachCell(cell) == false) return;
	_cells.pushBack(cell);
	// 设置楼层
	cell->setFloor(this);
	cell->setBlockFunc(CC_CALLBACK_1(Floor::isBlockBySite,this));
	// 添加碰撞检测单元
	cell->setSite(site);
	// 添加显示对象
	_stage->getChildByTag(layerId)->addChild(cell);
	addShade(cell->getShader());
}

bool Floor::removeCell(Cell* cell)
{
	if(Detect::shareDetect()->detachCell(cell) == false) return false;
	_cells.eraseObject(cell);
	// 删除碰撞检测
	cell->setSite(Vec2(-1,-1));
	// 删除楼层
	cell->setFloor(nullptr);
	cell->setBlockFunc(nullptr);
	// 删除显示对象
	removeShade(cell->getShader());
	cell->removeFromParent();
	return true;
}

void Floor::removeAllCells()
{
	/*for(auto pair:_units)
	{
		auto uCells = pair.second->getCells();
		Vector<Cell*> cells;
		for(auto cell:uCells)
		{
			cells.pushBack(cell);
		}
		uCells.clear();
		for(auto cell:cells)
		{
			cell->removeFromFloor();
		}
		cells.clear();
	}*/

	Vector<Cell*> cells;
	for(auto cell:_cells)
	{
		cells.pushBack(cell);
	}
	for(auto cell:cells)
	{
		cell->removeFromFloor();
	}
	cells.clear();
	/*for(auto pair:_units)
	{
	auto unit = pair.second;
	auto cells = unit->getCells();
	for(auto cell:cells)
	{
	if(cell->getName() == "player")
	{
	log("dfdfdf");
	}
	Detect::shareDetect()->detachCell(cell);
	cell->destroy();
	removeShade(cell->getShader());
	}
	unit->clearCells();
	}*/
}

//bool Floor::removeCellByType(int type)
//{
//	for(auto pair:_units)
//	{
//		auto unit = pair.second;
//		auto cells = unit->getCells();
//		for(auto cell:cells)
//		{
//			if(cell->getTypeId() == type)
//			{
//				unit->detach(cell);
//				cell->removeFromParent();
//				removeShade(cell->getShader());
//				return true;
//			}
//		}
//	}
//	return false;
//}
//
//bool Floor::removeCellByType(int type, Vec2 site)
//{
//	if(site.x < 0 || site.x >= HORIZON_TILE_NUM || site.y < 0 || site.y >= VERTICAL_TILE_NUM)
//		return false;
//	auto unit = _units.at(site.x + site.y * HORIZON_TILE_NUM);
//	auto cells = unit->getCells();
//	for(auto cell:cells)
//	{
//		if(cell->getTypeId() == type)
//		{
//			unit->detach(cell);
//			cell->removeFromParent();
//			removeShade(cell->getShader());
//			return true;
//		}
//	}
//	return false;
//}

// 1:墙壁、2、物件、10、超范围
int Floor::isBlockBySite(const Point &p)
{
	//log("%d",_id);
	int bRet = 0;
	ssize_t count = _map->getChildrenCount() - 1;
	if(p.x >= HORIZON_TILE_NUM || p.y >= VERTICAL_TILE_NUM || p.x < 0 || p.y < 0)
	{
		return 10;
	}
	
	auto block2 = _units.at(p.x + p.y * HORIZON_TILE_NUM)->isBlock();
	if(block2 > 0)
	{
		bRet = block2;
	}
	else
	{
		TMXLayer* layer;
		for(int i = 0; i < count;i++)
		{
			layer = (TMXLayer*)(_map->getChildByTag(i));
			Vec2 center = Vec2(p.x,VERTICAL_TILE_NUM - 1 - p.y);
			Vec2 point[] = {
				Vec2(2 * center.x, 2 * center.y + 1),
				Vec2(2 * center.x, 2 * center.y),
				Vec2(2 * center.x, 2 * center.y + 1),
				Vec2(2 * center.x + 1, 2 * center.y + 1)
			};
			for(int j = 0; j < 4; j++)
			{
				auto gid = layer->getTileGIDAt(point[j]);
				if(gid > 0)
				{
					auto value = _map0->getPropertiesForGID(gid);
					if(!value.isNull())
					{
						auto block = value.asValueMap()["block"].asInt();
						if(block > 0)
						{
							//log("%f, %f",p.x, p.y);
							bRet = block;
						}
					}				
				}
			}
		}
	}
	// 破墙
	if(bRet == 100)
		bRet = 0;
	//log("GID:%i, Properties:%s", b);
	return bRet;
}

Floor::~Floor()
{
	_eventDispatcher->removeEventListener(_listener);
}

const int Floor::getId() const
{
	return _id;
}

std::string Floor::getType()
{
	return "Floor";
}

void Floor::onEnter()
{
	Layer::onEnter();	
	
}

void Floor::onExit()
{
	Layer::onExit();
}

const std::string Floor::getNameId()
{
	return _model["name"].asString();
}

Vector<Cell*> Floor::getAllActor()
{
	Vector<Cell*> vec;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto actor = dynamic_cast<Actor*>(cell);
			if(actor != nullptr)
			{
				vec.pushBack(cell);
			}
		}
	}
	return vec;
}

Vector<Cell*> Floor::getAllItem()
{
	Vector<Cell*> vec;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto item = dynamic_cast<Item*>(cell);
			if(item != nullptr)
			{
				vec.pushBack(cell);
			}
		}
	}
	return vec;
}

Map<int, Cell*> Floor::getAllActorType()
{
	Map<int, Cell*> map;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto actor = dynamic_cast<Actor*>(cell);
			if(actor != nullptr && map.at(actor->getTypeId()) == nullptr)
			{
				map.insert(actor->getTypeId(),cell);
			}
		}
	}
	return map;
}

Map<int, Cell*> Floor::getAllItemType()
{
	Map<int, Cell*> map;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto item = dynamic_cast<Item*>(cell);
			if(item != nullptr && map.at(item->getTypeId()) == nullptr)
			{
				map.insert(item->getTypeId(),cell);
			}
		}
	}
	return map;
}

void Floor::attachToUnit(Cell* cell, Vec2 site)
{
	//auto newSite = PointFromString((*data)["effect"].asValueMap()["newSite"].asString());
	if(site.x >= HORIZON_TILE_NUM || site.x < 0 || site.y >= VERTICAL_TILE_NUM || site.y < 0)
		return;
	auto unit = _units.at(site.x + site.y * HORIZON_TILE_NUM);
	unit->attach(cell);
}

void Floor::detachFromUnit(Cell* cell, Vec2 site)
{
	//auto lastSite = PointFromString((*data)["effect"].asValueMap()["lastSite"].asString());
	if(site.x >= HORIZON_TILE_NUM || site.x < 0 || site.y >= VERTICAL_TILE_NUM || site.y < 0)
		return;
	auto unit = _units.at(site.x + site.y * HORIZON_TILE_NUM);
	unit->detach(cell);
}

void Floor::centerToUnit(Cell* cell, Vec2 site)
{
	//auto site = PointFromString((*data)["effect"].asValueMap()["newSite"].asString());
	auto unit = _units.at(site.x + site.y * HORIZON_TILE_NUM);
	unit->center(cell);
}

void Floor::touchUnit(Cell* cell, Vec2 site)
{
	if(site.x >= HORIZON_TILE_NUM || site.y >= VERTICAL_TILE_NUM || site.x < 0 || site.y < 0)
		return;
	auto unit = _units.at(site.x + site.y * HORIZON_TILE_NUM);
	unit->touch(cell);
}

Unit* Floor::getUnit(Vec2 site)
{
	return _units.at(site.x + site.y * HORIZON_TILE_NUM);
}

bool Floor::onTrigger(EventCustom *event)
{
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	if((*data)["type"].asString() == "addShade")
	{
		auto fShade = effect["fShade"].asString();
		auto fMask = effect["fMask"].asString();
		Vec2 site(0, 0);
		if(!effect["site"].isNull())
		{
			site = PointFromString(effect["site"].asString());
		}
		Vec2 pos(site.x * TILE_WIDTH + TILE_HALF_WIDTH, site.y * TILE_HEIGHT + TILE_HALF_HEIGHT);
		addShadeAt(fShade, fMask, pos);
	}
	return false;
}

#if DETECT_ACTOR == 1
void Floor::updateFight()
{
	if(Detect::shareDetect()->lookActorOpen == false)
		return;
	auto player = Detect::shareDetect()->getPlayer();
	if(player == nullptr) return;
	for(auto unit:_units)
	{
		for(auto cell:unit.second->getCells())
		{
			auto monster = dynamic_cast<Monster*>(cell);
			if(monster == nullptr) continue;
			auto mhit = monster->getStr() - player->getDef();
			auto phit = player->getStr() - monster->getDef();
			mhit = mhit > 0 ? mhit:0;
			phit = phit > 0 ? phit:0;
			int mround = phit == 0 ? 10000000:ceil(monster->getHp() * 1.0 / phit);
			std::string win;
			auto damage = mhit * (mround - 1);
			win = damage > player->getHp() ? "lose" : "win";
			std::string damageStr = phit == 0 ? a2u("over") : cocos2d::Value(damage).asString();
			monster->setDamageLabel(damageStr, win);
			
			/*if(phit != 0)
			{
				monster->setNumLabel(cocos2d::Value(mround - 1).asString());
				monster->setNum2Label(cocos2d::Value(phit).asString());


				auto nextRound = mround - 1;
				if(nextRound > 0)
				{
					while(ceil(monster->getHp() * 1.0 / nextRound) == ceil(monster->getHp() * 1.0 / (nextRound + 1)))
					{
						nextRound--;
						if(nextRound <= 0)
						{
							break;
						}
					}
				}

				if(nextRound > 0)
				{
					int nextPhit = ceil(monster->getHp() * 1.0 / nextRound);
					int gapStr = nextPhit - phit;
					int gapCount = mround - nextRound;
					int gapDamage = mhit * gapCount;

					monster->setNum3Label(cocos2d::Value(gapStr).asString() + "-" + cocos2d::Value(gapDamage).asString());
				}
				else
				{
					monster->setNum3Label("");
				}
			}*/
		}
	}
}
#endif
Unit* Unit::create(Vec2 vec)
{
	Unit *pRet = new Unit(vec);
	if (pRet)
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

void Unit::attach(Cell *cell)
{
	if(cell->getMutual() != -1)
	{
		for(int i = 0; i < _cells.size();i++)
		{
			auto c = _cells.at(i);
			if(c->getMutual() == -1)
				continue;
			EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:crashOn|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
			EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:crashOn|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());		
		}
	}
	_cells.pushBack(cell);
	cell->setUnit(this);
}

void Unit::detach(Cell *cell)
{
	_cells.eraseObject(cell);
	if(cell->getMutual() == -1)
		return;
	for(int i = 0; i < _cells.size();i++)
	{
		auto c = _cells.at(i);
		if(c->getMutual() == -1)
			continue;
		EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:crashOut|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
		EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:crashOut|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
	}
}

void Unit::center(Cell *cell)
{
	if(cell->getMutual() == -1)
		return;
	for(int i = 0; i < _cells.size();i++)
	{
		auto c = _cells.at(i);
		if(c == cell) continue;
		if(c->getMutual() == -1)
			continue;
		//EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:stepOn|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
		EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:stepOn|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());		
	}
}

void Unit::touch(Cell *cell)
{
	if(cell->getMutual() == -1)
		return;
	for(int i = 0; i < _cells.size();i++)
	{
		auto c = _cells.at(i);
		if(c->getMutual() == -1)
			continue;
		EventHelper::getInstance()->dispatchByStr("target:"+c->getName()+"|type:touchOn|senderType:"+cell->getType()+"|senderName:"+cell->getName()+"|senderTypeId:"+cocos2d::Value(cell->getTypeId()).asString(),cell->getEffect());
		EventHelper::getInstance()->dispatchByStr("target:"+cell->getName()+"|type:touchOn|senderType:"+c->getType()+"|senderName:"+c->getName()+"|senderTypeId:"+cocos2d::Value(c->getTypeId()).asString(),c->getEffect());
	}
}

// 
int Unit::isBlock()
{
	int bRet = 0;
	if(_cells.size() == 0)
	{
		return bRet;
	}
	for(auto cell:_cells)
	{
		auto block = cell->isBlock();
		if(block > 0)
		{
			if(bRet < block)
			{
				bRet = block;
			}
		}
	}
	return bRet;
}

Vector<Cell*> Unit::getCells()
{
	return _cells;
}

void Unit::clearCells()
{
	_cells.clear();
}

void Floor::buildMap()
{
	if(_mapBuilded == true)
		return;
	auto weather = _map->getProperty("weather");
	
	if(!weather.isNull())
	{

	}
	
	auto dark = _map->getProperty("dark");
	if(!dark.isNull())
	{
		ValueMap darkMap;
		stringToValueMap(dark.asString(),darkMap);
		_dark->setVisible(true);
		if(!darkMap["color"].isNull())
		{
			auto color = stringToColor4B(darkMap["color"].asString());
			_pLayer->setColor(Color3B(color));
			_pLayer->setOpacity(color.a);
		}
	}
	else
	{
		_dark->setVisible(false);
	}

	auto rain = _map->getProperty("rain");
	if(!rain.isNull())
	{
		ValueMap rainMap;
		stringToValueMap(rain.asString(),rainMap);
		ParticleSystemQuad* rainEmitter = ParticleSystemQuad::create("particles/rain.plist");
		rainEmitter->retain();
		rainEmitter->setName("rain");
		addChild(rainEmitter ,999);
	}

	_light = Layer::create();
	auto lighting = _map->getProperty("lighting");
	if(!lighting.isNull())
	{
		_lightColor = LayerColor::create(Color4B::WHITE);
		_light->addChild(_lightColor);
		_lightColor->setOpacity(0);
		//定时操作，来设置闪电 开关  一调用就说明要产生 一根闪电
		_lightDraw = DrawNode::create();
		_light->addChild(_lightDraw);

		_lightAction = Sequence::create(FadeTo::create(0.0f,230),Blink::create(0.4, 2),CallFunc::create([=](){_lightDraw->clear();}),FadeTo::create(0.5f,0),nullptr);
		_lightAction->retain();

		this->schedule(schedule_selector(Floor::lightUpdate),1.0f);
	}
	addChild(_light,1001);

	/*auto smoke = _map->getProperty("smoke");
	if(!smoke.isNull())
	{
	ValueMap smokeMap;
	stringToValueMap(smoke.asString(),smokeMap);
	ParticleSystemQuad* smokeEmitter = ParticleSystemQuad::create("particles/smoke.plist");
	smokeEmitter->retain();
	smokeEmitter->setName("smoke");
	addChild(smokeEmitter ,10);
	}*/

	/*auto rainDrop = _map->getProperty("rainDrop");
	if(!rainDrop.isNull())
	{
	ValueMap rainDropMap;
	stringToValueMap(rainDrop.asString(),rainDropMap);
	ParticleSystemQuad* rainDropEmitter = ParticleSystemQuad::create("particles/rainDrop.plist");
	rainDropEmitter->retain();
	rainDropEmitter->setName("rainDrop");
	addChild(rainDropEmitter ,10);
	}*/

	/*auto fog = _map->getProperty("fog");
	if(!fog.isNull())
	{
	ValueMap fogMap;
	stringToValueMap(fog.asString(),fogMap);
	ParticleSystemQuad* fogEmitter = ParticleSystemQuad::create("particles/fog.plist");
	fogEmitter->retain();
	fogEmitter->setName("fog");
	addChild(fogEmitter ,998);
	}*/
	//3D 波浪效果
	//震动快慢, ,波浪数,波浪振幅
	//static CCWaves3D* create(float duration, const CCSize& gridSize, unsigned int waves, float amplitude);
	//CCWaves3D* action = CCWaves3D::create(5.0f, CCSizeMake(5,10), 5, 5);

	//this->runAction(CCRepeatForever::create(action));
	//_emitter->setTexture( Director::getInstance()->getTextureCache()->addImage("image/fire.png") );

	/*auto s = Director::getInstance()->getWinSize();
	if (_emitter != nullptr)
	{
		_emitter->setPosition( Vec2(s.width / 2, s.height / 2) );
	}
*/

	// 动画
	//schedule(schedule_selector(Floor::updateMap), 0.2f);
	//scheduleUpdate();

	

	ssize_t count = _map->getChildrenCount() - 1;
	auto size = _map->getMapSize();

	for(int i = 0; i < count;i++)
	{
		TMXLayer* layer = (TMXLayer*)(_map->getChildByTag(i));
		for(int i = 0; i < size.width; i++)
		{
			for(int j = 0; j < size.height; j++)
			{					
				auto gid = layer->getTileGIDAt(Vec2(i,j));
				if(gid > 0)
				{
					auto value = _map0->getPropertiesForGID(gid);
					if(!value.isNull())
					{
						auto type = value.asValueMap()["type"].asString();
						auto id = value.asValueMap()["id"].asInt();

						auto name = value.asValueMap()["name"].asString();
						auto anis = value.asValueMap()["anis"].asString();
						auto duration = value.asValueMap()["duration"].asFloat();
						if(duration == 0) duration = 0.35;
						if(anis != "")
						{
							auto sprite = layer->getTileAt(Vec2(i,j));
							ValueVector temp;
							ValueVector anisVec;
							stringToValueVector(anis,",",temp);
							for(auto ani:temp)
							{
								anisVec.push_back(cocos2d::Value(int(gid + ani.asInt())));
							}
							sprite->runAction(RepeatForever::create(AnimateByGID::create(anisVec,layer,Vec2(i,j),duration)));
						}
					}
				}
			}
		}
	}

	// 抗锯齿
	SpriteBatchNode* child = nullptr;
	auto& children = _map->getChildren();
	for(const auto &node : children) 
	{
	child = dynamic_cast<SpriteBatchNode*>(node);
	if(child == nullptr)
	continue;
	child->getTexture()->setAliasTexParameters();
	}

	_mapBuilded = true;
}

void Floor::stopWeather()
{
	this->unschedule(schedule_selector(Floor::lightUpdate));

	auto node1 = this->getChildByName("fog");
	if(node1 != nullptr) removeChild(node1);

	auto node2 = this->getChildByName("rainDrop");
	if(node2 != nullptr) removeChild(node2);

	auto node3 = this->getChildByName("smoke");
	if(node3 != nullptr) removeChild(node3);

	auto node4 = this->getChildByName("rain");
	if(node4 != nullptr) removeChild(node4);

	_pLayer->setOpacity(0);
}

Cell* Floor::getNearByTypeId(Vec2 center,const Vec2* vecs, int len1,const int* typeIds, int len2)
{
	for(int i = 0; i < len1; i++)
	{
		auto site = center + vecs[i];
		if(site.x >= HORIZON_TILE_NUM || site.y >= VERTICAL_TILE_NUM || site.x < 0 || site.y < 0)
			continue;
		auto cells = getUnit(site)->getCells();
		for(auto cell:cells)
		{
			for(int j = 0; j < len2; j++)
			{
				if(cell->getTypeId() == typeIds[j])
					return cell;
			}
		}
	}
	return nullptr;
}

Cell* Floor::getNearByType(Vec2 center, const Vec2* vecs, int len1,const std::string* types, int len2)
{
	for(int i = 0; i < len1; i++)
	{
		auto site = center + vecs[i];
		if(site.x >= HORIZON_TILE_NUM || site.y >= VERTICAL_TILE_NUM || site.x < 0 || site.y < 0)
			continue;
		auto cells = getUnit(site)->getCells();
		for(auto cell:cells)
		{
			for(int j = 0; j < len2; j++)
			{
				if(cell->getType() == types[j])
					return cell;
			}
		}
	}
	return nullptr;
}