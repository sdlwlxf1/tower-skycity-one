#include "Actor.h"
#include "Global.h"
#include "utils/AStar.h"
#include "TObject.h"
#include "utils/StringHandler.h"
#include "scene/GameScene.h"
#include "base/CCNS.h"
#include "model/Model.h"
#include "editor-support/cocostudio/DictionaryHelper.h"
#include "mutual/FightNew.h"
#include "Detect.h"
#include "mutual/ChatManager.h"
#include "UI/PopPanel.h"
#include "mutual/Swap.h"
#include "ui/DiaryPanel.h"
#include "utils/PlotScript.h"
#include "cell/CellFactory.h"
#include "ui/PaperPanel.h"
#include "ui/LevelUpPanel.h"
#include "Utils/Tools.h"

USING_NS_CC;
//KB,PJ,BJ,XX,LJ,RD,RH,GD,XS,SB
const char* Actor::FightBuffStr[10] = {"狂暴","破甲","暴击","吸血","连击","肉盾","弱化","格挡","反弹","闪避"};
const char* Actor::FightBuffRateStr[10] = {"kbr","pjr","bjr","xxr","ljr","rdr","rhr","gdr","ftr","sbr"};
const char* Actor::FightBuffDrateStr[10] = {"kbdr","pjdr","bjdr","xxdr","ljdr","rddr","rhdr","gddr","ftdr","sbdr"};
std::string Actor::MovementStr[5] = {"stop","left","up","right","down"};
std::string Actor::AnimationTypeStr[3] = {"idle","walk","hit"};
std::map<const char*, int> Actor::FightBuffRateStrToInt;
std::map<const char*, int> Actor::FightBuffDrateStrToInt;

void Actor::buildMap()
{
	for(int i = 0; i < 10; i++)
	{
		FightBuffRateStrToInt[FightBuffRateStr[i]] = i;
	}
	for(int i = 0; i < 10; i++)
	{
		FightBuffDrateStrToInt[FightBuffDrateStr[i]] = i;
	}
}

Actor* Actor::create(int id)
{
	Actor *pRet = new Actor(id);
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

Actor* Actor::create(ValueMap& model)
{
	Actor *pRet = new Actor(model);
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

Actor::Actor(ValueMap& model)
{
	_model = model;
	auto value = _model["typeId"];
	if(!value.isNull())
	{
		_typeId = value.asInt();
	}
	else
	{
		_typeId = 1;
	}
}

Actor::Actor(int typeId)
{
	_typeId = typeId;
}

int Actor::getId()
{
	return _typeId;
}


void Actor::initModel()
{
	Cell::initModel();
	// 初始化数据
	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->actorDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->actorCsv->getModelById(typeId);
		Model::modelAddModel(model,_model);
	}
}

void Actor::refreshModel()
{
	Cell::refreshModel();
	// 初始化数据
	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->actorDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->actorCsv->getModelById(typeId);
		Model::modelCoverModel(model,_model);
	}
}

void Actor::loadModel(ValueMap &map)
{
	Cell::loadModel(map);
}

ValueMap Actor::saveModel()
{
	ValueMap model = Cell::saveModel();
	model["nickName"] = _nickName;
	model["hp"] = _hp;
	model["str"] = _str;
	model["def"] = _def;
	model["xp"] = _xp;
	model["level"] = _level;
	model["gold"] = _gold;
	model["move"] = _move;
	model["speed"] = _speed;

	ValueVector fightBuffRate;
	for(auto value:_fightBuffRate)
	{
		fightBuffRate.push_back(cocos2d::Value(value));
	}
	model["fightBuffRate"] = fightBuffRate;
	ValueVector fightBuffDrate;
	for(auto value:_fightBuffDrate)
	{
		fightBuffDrate.push_back(cocos2d::Value(value));
	}
	model["fightBuffDrate"] = fightBuffDrate;

	model["chatId"] = _chatId;
	model["conveyCount"] = _conveyCount;
	model["swapCount"] = _swapCount;
	model["relation"] = _model["relation"];
	model["dealAppear"] = _model["dealAppear"];
	model["swapId"] = _model["swapId"];
	model["noDead"] = _model["noDead"];
	model["isDead"] = _isDead;
	return model;
}

void Actor::initOthers()
{
	// 动画
	_animations = ValueMap();
	stringToValueMap(_model["animation"].asString(), _animations);

	// 属性赋值
	if(_model["level"].isNull() || _refresh == true)
	{
		_model["level"] = _model["level_init"];
	}
	_level = _model["level"].asInt();
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas1 = Model::shareModel()->levelDatas;
		auto &data = Model::GetDataFromCol1(datas1,_level);
		_maxHp = DICHELPER->getIntValue_json(data,"maxHp");

		rapidjson::Document &datas2 = Model::shareModel()->levelDatas;
		auto &lastData = Model::GetDataFromCol1(datas2,_level);
		_lastXp = DICHELPER->getIntValue_json(lastData,"xp");

		rapidjson::Document &datas3 = Model::shareModel()->levelDatas;
		auto &nextData = Model::GetDataFromCol1(datas3,_level + 1);
		_nextXp = DICHELPER->getIntValue_json(nextData,"xp");
	}
	else if(DATA_MODE == 2)
	{
		auto models = Model::shareModel()->levelCsv;
		_maxHp = models->getValueByIdAndName(_level,"maxHp").asInt();
		_lastXp = models->getValueByIdAndName(_level,"xp").asInt();
		_nextXp = models->getValueByIdAndName(_level + 1,"xp").asInt();
	}

	// 初始化数据
	if(_model["hp"].isNull() || _refresh == true)
	{
		_model["hp"] = _model["hp_init"];
	}
	if(_model["str"].isNull() || _refresh == true)
	{
		_model["str"] = _model["str_init"];
	}
	if(_model["def"].isNull() || _refresh == true)
	{
		_model["def"] = _model["def_init"];
	}
	if(_model["xp"].isNull() || _refresh == true)
	{
		_model["xp"] = _model["xp_init"];
	}
	if(_model["level"].isNull() || _refresh == true)
	{
		_model["level"] = _model["level_init"];
	}
	if(_model["gold"].isNull() || _refresh == true)
	{
		_model["gold"] = _model["gold_init"];
	}
	if(_model["speed"].isNull() || _refresh == true)
	{
		_model["speed"] = _model["speed_init"];
	}

	if(_model["chatId"].isNull() || _refresh == true)
	{
		_model["chatId"] = _model["chatId_init"];
	}

	_hp = _model["hp"].asInt();
	_str = _model["str"].asInt();
	_def = _model["def"].asInt();
	_level = _model["level"].asInt();
	_xp = _model["xp"].asInt();
	_gold = _model["gold"].asInt();
	_speed = _model["speed"].asFloat();
	_chatId = _model["chatId"].asInt();
	

	if(_model["fightBuffRate"].isNull() || _refresh == true)
	{
		ValueVector fightBuffRate;
		stringToValueVector(_model["fightBuffRate_init"].asString(),",",fightBuffRate);
		_model["fightBuffRate"] = fightBuffRate;
	}
	if(_model["fightBuffDrate"].isNull() || _refresh == true)
	{
		ValueVector fightBuffDrate;
		stringToValueVector(_model["fightBuffDrate_init"].asString(),",",fightBuffDrate);
		_model["fightBuffDrate"] = fightBuffDrate;
	}

	for(int i=0;i<10;i++)
	{
		_fightBuffRate[i] = _model["fightBuffRate"].asValueVector().at(i).asFloat();
	}
	for(int i=0;i<10;i++)
	{
		_fightBuffDrate[i] = _model["fightBuffDrate"].asValueVector().at(i).asFloat();
	}

	_nickName = _model["nickName"].asString();
	setName(_model["name"].asString());
	_conveyCount = 2;
	_swapCount = 0;
	
	if(!_model["conveyCount"].isNull())
	{
		_conveyCount = _model["conveyCount"].asInt();
	}
	if(!_model["swapCount"].isNull())
	{
		_swapCount = _model["swapCount"].asInt();
	}
	if(!_model["isDead"].isNull())
	{
		_isDead = _model["isDead"].asBool();
	}
	

	if(Detect::shareDetect()->viewHide == 1)
	{
		if(!_model["viewHide"].isNull())
		{

			if(_model["viewHide"].asInt() == 1)
			{
				this->setVisible(false);
			}
		}
	}

	_initTime = getTime();
	_durTime = 0;
	if(!_model["durTime"].isNull())
	{
		_durTime = _model["durTime"].asDouble();
	}

	Cell::initOthers();
}


void Actor::initNull()
{
	Cell::initNull();
	// 其他属性初始化
	_itemBag = nullptr;
	_sparDisk = nullptr;
	_diary = nullptr;
	_canWalk = true;
	_canGuide = true;
	_ani = IDLE;
	_face = DOWN;
	_move = STOP;
	_saveSpeed = 0;
	_isDead = false;
}

void Actor::setNickName(std::string nickName)
{
	_nickName = nickName;
}

void Actor::setHp(int hp)
{
	//_hp = (hp <= _maxHp)?hp:_maxHp;
	_hp = hp;
	_hp = (_hp <= 0)?0:_hp;
	if(_hp == 0)
	{
		dead();
	}
}

void Actor::setStr(int str)
{
	_str = str;	
}

void Actor::setDef(int def)
{
	_def = def;
}

void Actor::setXp(int xp)
{
	_xp = xp;
}

void Actor::setGold(int gold)
{
	_gold = gold;
}

void Actor::setSpeed(float speed)
{
	_speed = speed;
}

void Actor::setLevel(int level)
{
	_level = level;

	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas1 = Model::shareModel()->levelDatas;
		auto &data = Model::GetDataFromCol1(datas1,_level);
		_maxHp = DICHELPER->getIntValue_json(data,"maxHp");
		setHp(_hp);

		rapidjson::Document &datas2 = Model::shareModel()->levelDatas;
		auto &lastData = Model::GetDataFromCol1(datas2,_level);
		_lastXp = DICHELPER->getIntValue_json(lastData,"xp");

		rapidjson::Document &datas3 = Model::shareModel()->levelDatas;
		auto &nextData = Model::GetDataFromCol1(datas3,_level + 1);
		_nextXp = DICHELPER->getIntValue_json(nextData,"xp");
	}
	else if(DATA_MODE == 2)
	{
		auto models = Model::shareModel()->levelCsv;
		_maxHp = models->getValueByIdAndName(_level,"maxHp").asInt();
		setHp(_hp);
		_lastXp = models->getValueByIdAndName(_level,"xp").asInt();
		_nextXp = models->getValueByIdAndName(_level + 1,"xp").asInt();
	}
}

int Actor::getHp()
{
	return _hp + ((_itemBag == nullptr) ? 0 : _itemBag->getAddHp());
}
int Actor::getStr()
{
	return _str + ((_itemBag == nullptr) ? 0 : _itemBag->getAddStr());
}
int Actor::getDef()
{
	return _def + ((_itemBag == nullptr) ? 0 : _itemBag->getAddDef());
}

int Actor::getXp()
{
	return _xp;
}

float Actor::getSpeed()
{
	return _speed * Detect::shareDetect()->getUser()->getMultiSpeed();
}

void Actor::addFightBuffRate(FightBuff id, float num)
{
	auto value = _fightBuffRate[id] + num;
	setFightBuffRate(id,value);
}

void Actor::addFightBuffDrate(FightBuff id, float num)
{
	auto value = _fightBuffDrate[id] + num;
	setFightBuffDrate(id,value);
}

float Actor::getFightBuffRate(FightBuff id)
{
	auto value = _fightBuffRate[id] + ((_itemBag == nullptr) ? 0 : _itemBag->getAddFightBuffRate(int(id))) + ((_sparDisk == nullptr) ? 0 : _sparDisk->getAddFightBuffRate(int(id)));
	return value;
}

float Actor::getFightBuffDrate(FightBuff id)
{
	auto value = _fightBuffDrate[id] + ((_itemBag == nullptr) ? 0 : _itemBag->getAddFightBuffDrate(int(id))) + ((_sparDisk == nullptr) ? 0 : _sparDisk->getAddFightBuffDrate(int(id)));
	return value;
}

void Actor::setFightBuffRate(FightBuff id, float value)
{
	_fightBuffRate[id] = value;
}

void Actor::setFightBuffDrate(FightBuff id, float value)
{
	_fightBuffDrate[id] = value;
}


double Actor::getDurTime()
{
	auto time = getTime();
	return _durTime + (time - _initTime);
}

void Actor::addHp(int hp)
{
	auto value = _hp + hp;
	setHp(value);
}

void Actor::addStr(int str)
{
	auto value = _str + str;
	setStr(value);
}

void Actor::addDef(int def)
{
	auto value = _def + def;
	setDef(value);
}

void Actor::addXp(int xp)
{
	auto value = _xp + xp;

	int level;

	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->levelDatas;
		auto &data = Model::GetDataFromCol2(datas,value,1);
		level = DICHELPER->getIntValue_json(data,"level");
	}
	else if(DATA_MODE == 2)
	{
		auto models = Model::shareModel()->levelCsv;
		level = models->getModelByAlikeTypeAndValue("xp",cocos2d::Value(value))["level"].asInt();
	}

	setLevel(level);
	setXp(value);
}

void Actor::addLevel(int level)
{
	auto value = _level + level;
	setLevel(value);
}

void Actor::addGold(int gold)
{
	auto value = _gold + gold;
	setGold(value);
}

void Actor::addSpeed(float speed)
{
	auto value = _speed + speed;
	setSpeed(value);
}

Actor::~Actor()
{

}

void Actor::setSite(const Vec2 &vec)
{
	Cell::setSite(vec);
}

void Actor::moveToNewSite()
{
	Cell::moveToNewSite();
}

void Actor::stepToNewSite()
{
	Cell::stepToNewSite();
}

void Actor::initView()
{
	//initWithSpriteFrameName(_model["image"].asString());
	/*_num2Label = Label::createWithTTF("", "fonts/58983.ttf", 12);
	_num2Label->setColor(Color3B::WHITE);
	addChild(_num2Label);*/
	auto shadow = Sprite::createWithSpriteFrameName("shadow.png");
	shadow->setAnchorPoint(Point(0.5,0.25));
	addChild(shadow);

	Cell::initView();
	//animate(IDLE,_face);
#if DETECT_ACTOR == 1
	_damageLabel = Label::createWithTTF("", "fonts/58983.ttf", 13);
	_damageLabel->setColor(Color3B::WHITE);
	addChild(_damageLabel);
	_damageLabel->setPosition(Vec2(0,-10));
	_damageLabel->setAnchorPoint(Vec2(0.5, 0.5));
	_damageLabel->setVisible(false);
	_damageLabel->setString("");

	_numLabel = Label::createWithTTF("", "fonts/58983.ttf", 12);
	_numLabel->setColor(Color3B::WHITE);
	addChild(_numLabel);
	_numLabel->setPosition(Vec2(0,0));
	_numLabel->setAnchorPoint(Vec2(0.5, 0.5));
	_numLabel->setVisible(false);
	_numLabel->setString("");

	_num2Label = Label::createWithTTF("", "fonts/58983.ttf", 12);
	_num2Label->setColor(Color3B::WHITE);
	addChild(_num2Label);
	_num2Label->setPosition(Vec2(15,0));
	_num2Label->setAnchorPoint(Vec2(0.5, 0.5));
	_num2Label->setVisible(false);
	_num2Label->setString("");

	_num3Label = Label::createWithTTF("", "fonts/58983.ttf", 12);
	_num3Label->setColor(Color3B::WHITE);
	addChild(_num3Label);
	_num3Label->setPosition(Vec2(0,10));
	_num3Label->setAnchorPoint(Vec2(0.5, 0.5));
	_num3Label->setVisible(false);
	_num3Label->setString("");
	//_damageLabel->enableOutline(Color4B::GREEN,10);
	/*_winLabel = Text::create("","Arial",15);
	_winLabel->setColor(Color3B::WHITE);
	addChild(_winLabel);
	_winLabel->setPosition(Vec2(30,10));
	_winLabel->setAnchorPoint(Vec2(0.5, 0.5));
	_winLabel->setVisible(false);
	_winLabel->setString("");
	_winLabel->enableShadow(Color4B::RED,Size(1,-1),100);*/
#endif
}

#if DETECT_ACTOR == 1
void Actor::setDamageLabel(std::string damage, std::string win)
{
	_damageLabel->setVisible(true);
	_damageLabel->setString(damage);
	//_winLabel->setString(win);
	if(win == "win" && damage != "over")
	{
		//_damageLabel->enableShadow(Color4B::GREEN,Size(1,-1),100);
		_damageLabel->enableOutline(Color4B(120,196,0,255),1);
	}
	else
	{
		//_damageLabel->enableShadow(Color4B::RED,Size(1,-1),100);
		_damageLabel->enableOutline(Color4B(196,0,0,255),1);
	}
}

void Actor::setNumLabel(std::string num)
{
	_numLabel->setVisible(true);
	_numLabel->setString(num);
	_numLabel->enableOutline(Color4B(196,0,0,255),1);
}

void Actor::setNum2Label(std::string num)
{
	_num2Label->setVisible(true);
	_num2Label->setString(num);
	_num2Label->enableOutline(Color4B(196,0,0,255),1);
}

void Actor::setNum3Label(std::string num)
{
	_num3Label->setVisible(true);
	_num3Label->setString(num);
	_num3Label->enableOutline(Color4B(196,0,0,255),1);
}
#endif

void Actor::onEnter()
{
	Cell::onEnter();
	animate(IDLE,_face);
	/*setNickName(_nickName);
	setLevel(_level);
	setHp(_hp);
	setStr(_str);
	setDef(_def);
	setXp(_xp);
	setGold(_gold);
	moveToNewSite();*/

	scheduleUpdate();

}

void Actor::update(float delta)
{
	Cell::update(delta);
}

void Actor::animate(AnimationType type, Direction dir)
{
	auto str = StringUtils::format("%s_%s", AnimationTypeStr[type].c_str(), MovementStr[dir].c_str());
	if(setAnimation(str, true) == true)
	{
		_ani = type;
		_face = dir;
		_faceSite = _site + CrossVecs[_face];
	}
}

void Actor::setCanGuide(bool b, float delay)
{

}

bool Actor::onTrigger(EventCustom *event)
{
	Cell::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	if((*data)["type"].asString() == "face")
	{
		int dir = DOWN;
		if(!effect["dir"].isNull())
		{
			dir = effect["dir"].asInt();
		}
		else if(!effect["toCell"].isNull())
		{
			Vec2 site;
			auto cell = Detect::shareDetect()->getCellByName(effect["toCell"].asString());
			if(cell != nullptr) site = cell->getSite();
			int disX = site.x - _site.x;
			int dixY = site.y - _site.y;
			if(disX != 0)
				dir = disX > 0 ? RIGHT:LEFT;
			if(dixY != 0)
				dir = dixY > 0 ? UP:DOWN;
		}
		animate(_ani,Direction(dir));
	}
	else if((*data)["type"].asString() == "flyOff")
	{
		Vec2 position(0,0);
		float duration = 1;
		if(!effect["position"].isNull())
		{
			position = PointFromString(effect["position"].asString());
		}
		if(!effect["duration"].isNull())
		{
			duration = effect["duration"].asFloat();
		}
		this->runAction(MoveTo::create(duration,position));
	}
	else if((*data)["type"].asString() == "dead")
	{
		dead();
	}
	else if((*data)["type"].asString() == "isDead")
	{
		log("isDead");
	}
	return false;
}

int Actor::isBlock()
{
	return _model["isBlock"].asInt();
}

void Actor::dead()
{
	conditions[getName()+"_dead"] = LuaValue::intValue(1);
	conditions[getName()+"_isDead"] = LuaValue::intValue(1);
	setIsDead(true);
	if(getModelByName("noDead").isNull())
	{
		runAction(Sequence::create(DelayTime::create(0.2), CallFunc::create([=](){Detect::shareDetect()->removeCell(this);}),nullptr));
	}
	EventHelper::getInstance()->dispatchByStr("target:"+getName()+"|type:isDead","");
}

void Actor::setPosition(const Vec2& pos)
{
	Layer::setPosition(pos);
	if(_shader != nullptr)
		_shader->update(pos);
}

Monster* Monster::create(ValueMap& model)
{
	Monster *pRet = new Monster(model);
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

bool Monster::onTrigger(EventCustom *event)
{
	Actor::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	return false;
}

Role* Role::create(int typeId)
{
	Role *pRet = new Role(typeId);
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

Role* Role::create(ValueMap& model)
{
	Role *pRet = new Role(model);
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

void Role::initModel()
{
	Actor::initModel();	
}

void Role::loadModel(ValueMap &map)
{
	Actor::loadModel(map);
	_useBombNum = _model["useBombNum"].asInt();
	_useHoeNum = _model["useHoeNum"].asInt();
}

ValueMap Role::saveModel()
{
	auto model = Actor::saveModel();
	model["itemBag"] = _itemBag->saveModel();
	model["diary"] = _diary->saveModel();
	model["sparDisk"] = _sparDisk->saveModel();

	model["useBombNum"] = _useBombNum;
	model["useHoeNum"] = _useHoeNum;
	return model;
}

void Role::initOthers()
{
	Actor::initOthers();

	_useBombNum = 0;
	if(!_model["useBombNum"].isNull())
	{
		_useBombNum = _model["useBombNum"].asInt();
	}
	_useHoeNum = 0;
	if(!_model["useHoeNum"].isNull())
	{
		_useHoeNum = _model["useHoeNum"].asInt();
	}

	if(_refresh == false)
	{
		if(!_model["itemBag"].isNull())
		{
			_itemBag = ItemBag::create(_model["itemBag"].asValueMap());
			_itemBag->setOwner(this);
		}
		else
		{
			_itemBag = ItemBag::create();
			_itemBag->setOwner(this);
		}
		CC_SAFE_RETAIN(_itemBag);


		if(!_model["sparDisk"].isNull())
		{
			_sparDisk = SparDisk::create(_model["sparDisk"].asValueMap());
			_sparDisk->setOwner(this);
		}
		else
		{
			_sparDisk = SparDisk::create();
			_sparDisk->setOwner(this);
		}
		CC_SAFE_RETAIN(_sparDisk);

		if(!_model["diary"].isNull())
		{
			_diary = DiaryPanel::create("diary", _model["diary"].asValueMap());
			_diary->setOwner(this);
		}
		else
		{
			_diary = DiaryPanel::create("diary");
			_diary->setOwner(this);
		}
		CC_SAFE_RETAIN(_diary);
	}
}

void Role::initNull()
{
	Actor::initNull();
	_paths = nullptr;
	_guide = STOP;
	_canWalk = true;
	_canGuide = true;
	_vision = PointArray::create(5);
	CC_SAFE_RETAIN(_vision);
	_astar = AStar::create();
}

Role::~Role()
{
	CC_SAFE_RELEASE(_diary);
	CC_SAFE_RELEASE(_sparDisk);
	CC_SAFE_RELEASE(_itemBag);
	CC_SAFE_RELEASE(_vision);
	CC_SAFE_RELEASE(_paths);
}


void Role::onEnter()
{
	Actor::onEnter();
}

void Role::setBlockFunc(std::function<int(Vec2&)> func)
{
	Cell::setBlockFunc(func);
	_astar->initCheckPointFunc(func);
}

void Role::moveTo(Vec2 dest)
{
	CC_SAFE_RELEASE(_paths);
	/*Vec2 origin = _site;
	_paths = _astar->getShortPath(origin,dest);

	Vec2 vec2 = this->getPosition();
	if((_move == RIGHT && (int(vec2.x) % TILE_WIDTH > TILE_HALF_WIDTH))
	|| (_move == UP && (int(vec2.y) % TILE_HEIGHT > TILE_HALF_HEIGHT))
	|| (_move == LEFT && (int(vec2.x) % TILE_WIDTH < TILE_HALF_WIDTH))
	|| (_move == DOWN && (int(vec2.y) % TILE_HEIGHT < TILE_HALF_HEIGHT)))
	{
	origin = _nextSite;
	}*/

	//_paths = _astar->getShortPath(origin,dest);
	//changeGuide();
	//auto removeSite = changeGuide();
	//if (_guide != STOP && _move != reverse(_guide) && _move != STOP && !checkCenterTile()/* && removeSite != Vec2()*/)
	//{
	//	_paths->insertControlPoint(removeSite, 0);
	//}
	Vec2 vec = this->getPosition();
	Point site;
	vec = (vec - Vec2(TILE_HALF_WIDTH, TILE_HALF_HEIGHT)) * 0.025;
	site.setPoint(int(vec.x), int(vec.y));
	Vec2 nextSite = site + _dis;

	_paths = _astar->getShortPath((_move == UP || _move ==RIGHT)?nextSite:site,dest);
	auto removeSite = changeGuide();
	if (_guide != STOP && _move != reverse(_guide) && _move != STOP && !checkCenterTile()/* && removeSite != Vec2()*/)
	{
		_paths->insertControlPoint(removeSite, 0);
	}
	CC_SAFE_RETAIN(_paths);
}

float Role::getFightDelay()
{
	return _fightDelay * Detect::shareDetect()->getUser()->getMultiFightDelay();
}

Vec2 Role::changeGuide()
{	
	if(_paths->count() > 1)
	{
		Point curSite;
		Point nextSite;
		curSite = _paths->getControlPointAtIndex(0);
		_paths->removeControlPointAtIndex(0);
		nextSite = _paths->getControlPointAtIndex(0);

		Point p = nextSite - curSite;
		if(p == CrossVecs[1])
		{
			_guide = LEFT;
		}else if(p == CrossVecs[2])
		{
			_guide = UP;
		}else if(p == CrossVecs[3])
		{
			_guide = RIGHT;
		}else if(p == CrossVecs[4])
		{
			_guide = DOWN;
		}
		return curSite;
	}
	return Vec2();
}

void Role::update(float delta)
{
	Actor::update(delta);
	updateGuide();
	if(_canWalk)
	{
		walk();
	}	
}

void Role::walk()
{
	if(_guide != STOP)
	{
		if (nextTileCanMove(_guide) && (_move == _guide||_move == reverse(_guide)||_move == STOP||checkCenterTile()))
		{
			switchMovement(_guide);
			//playSound = true;
		}
		else if(_move == STOP)
		{
			animate(IDLE, _guide);
		}
	}

	if(_dis.isZero() == true)
	{
		animate(IDLE, _face);
	}
	else
	{
		animate(WALK, _face);
	}

	Vec2 vec = this->getPosition();

	vec.x += _dis.x * getSpeed();
	vec.y += _dis.y * getSpeed();// * / 40

	this->setPosition(vec);

	Vec2 newSite;
	vec *= 0.025;
	newSite.setPoint(floor(vec.x), floor(vec.y));
	if(newSite != _site)
	{
		_lastSite = _site;
		_site = newSite;
		_conveyCount++;
		moveToNewSite();
	}

	if (checkCenterTile()) 
	{
		_guide = STOP;
		if(_paths != nullptr && _paths->getControlPointAtIndex(0) == _site)
		{
			changeGuide();
		}
		switchMovement(STOP);
	}
}

void Role::moveToNewSite()
{
	Actor::moveToNewSite();
}

void Role::stepToNewSite()
{
	Actor::stepToNewSite();
}

//检查坐标在不在方块的中心
bool Role::checkCenterTile()
{
	Vec2 center(((_site.x * TILE_WIDTH) + TILE_HALF_WIDTH), ((_site.y * TILE_HEIGHT) + TILE_HALF_HEIGHT));
	auto vec = this->getPosition();
	if(center == vec)
	{
		if(_centerSite != _site)
		{
			_centerSite = _site;
			stepToNewSite();
		}
		return true;
	}
	return false;
}

//运动吧
void Role::switchMovement(Movement movement)
{
	_dis.set(CrossVecs[int(movement)]);
	_move = movement;
	if(movement != STOP)
	{
		animate(_ani, Direction(movement));
	}
	_nextSite = _site + _dis;
}


void Role::setCanGuide(bool b, float delay)
{
	for(int i = 0; i < 5;i++)
	{
		_guides[i] = 0;
	}
	if(delay != 0)
	{
		auto antion = Sequence::create(DelayTime::create(delay), CallFunc::create([=](){setCanGuide(b);}), NULL);
		this->runAction(antion);
		return;
	}
	if(b == false)
	{
		_guide = STOP;
		_canGuide = false;
	}
	else if(b == true)
	{
		_canGuide = true;
	}
}


void Role::addPropByType(int typeId, int num)
{
	while(num--)
	{
		ValueMap model;
		model["typeId"] = typeId;
		auto prop = Prop::create(model);
		addProp(prop);
	}
}

void Role::addEquipByType(int typeId)
{
	ValueMap model;
	model["typeId"] = typeId;
	auto equip = Equip::create(model);
	addEquip(equip);
}

void Role::addSpecialByType(int typeId, int num)
{
	while(num--)
	{
		ValueMap model;
		model["typeId"] = typeId;
		auto special = Special::create(model);
		addSpecial(special);
	}	
}

void Role::addSparByType(int typeId)
{
	ValueMap model;
	model["typeId"] = typeId;
	auto spar = Spar::create(model);
	addSpar(spar);
}

void Role::removePropByType(int typeId, int num)
{
	_itemBag->removePropByType(typeId, num);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getPropSizeByType(typeId));
}

void Role::removeSpecialByType(int typeId, int num)
{
	_itemBag->removeSpecialByType(typeId, num);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getSpecialSizeByType(typeId));
}

void Role::addEquip(Equip *equip)
{
	int typeId = equip->getTypeId();
	std::string name = equip->getName();
	_itemBag->addEquip(equip);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(1);
}

void Role::addSpar(Spar *spar)
{
	int typeId = spar->getTypeId();
	std::string name = spar->getName();
	_sparDisk->addSpar(spar);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(1);
}

void Role::addProp(Prop *prop)
{
	int typeId = prop->getTypeId();
	std::string name = prop->getName();
	_itemBag->addProp(prop);
	conditions[getName() + "_add_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getPropSizeByType(typeId));
}

void Role::addSpecial(Special *special)
{
	int typeId = special->getTypeId();
	std::string name = special->getName();
	_itemBag->addSpecial(special);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(1);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getSpecialSizeByType(typeId));
}

void Role::removeProp(Prop *prop)
{
	int typeId = prop->getTypeId();
	std::string name = prop->getName();
	_itemBag->removeProp(prop);
	conditions[getName() + "_has_" + name] = LuaValue::intValue(0);
	conditions[getName() + "_has_" + cocos2d::Value(typeId).asString() + "_num"] = LuaValue::intValue(_itemBag->getPropSizeByType(typeId));
}

//检查游戏对象的下一个方块是否可行
bool Role::nextTileCanMove(Direction dir)
{	
	//设置下一个方块行和列
	Vec2 vec = _site + CrossVecs[int(dir)];
	return !_blockFunc(vec);
}

bool Role::onTrigger(EventCustom *event)
{
	Actor::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	if((*data)["type"].asString() == "crashOn")
	{
		auto name = (*data)["senderName"].asString();
		// 捡到消费道具
		if((*data)["senderType"].asString() == "Prop")
		{
			log("I pick a Prop[%s]",(*data)["senderName"].asString().c_str());
			auto prop = dynamic_cast<Prop*>(Detect::shareDetect()->getCellByName(name));
			if(prop != nullptr) addProp(prop);
		}
		// 捡到装备
		else if((*data)["senderType"].asString() == "Equip")
		{
			log("I pick a Equip[%s]",(*data)["senderName"].asString().c_str());
			auto equip = dynamic_cast<Equip*>(Detect::shareDetect()->getCellByName(name));
			if(equip == nullptr) return false;
			auto ownEquip = _itemBag->getEquipByType(equip->getTypeId1());
			if(ownEquip != nullptr && ownEquip->getTypeId() > equip->getTypeId())
			{
				PopPanel::getInstance()->note("cantEquip","type:Text---text:"+ a2u("你已经拥有了更高级的装备"));
			}
			else
			{
				addEquip(equip);
			}
		}
		// 捡到特殊道具
		else if((*data)["senderType"].asString() == "Special")
		{
			log("I pick a Special[%s]",(*data)["senderName"].asString().c_str());
			auto special = dynamic_cast<Special*>(Detect::shareDetect()->getCellByName(name));
			if(special != nullptr) addSpecial(special);
		}
		// 捡到一次性道具
		else if((*data)["senderType"].asString() == "Gem")
		{
			log("I use a Gem[%s]",(*data)["senderName"].asString().c_str());
			auto typeId = (*data)["senderTypeId"].asInt();
			getGemByType(typeId);
		}
		// 捡到晶石
		else if((*data)["senderType"].asString() == "Spar")
		{
			log("I use a Gem[%s]",(*data)["senderName"].asString().c_str());
			if(_itemBag->getSpecialByType(2007001) == nullptr)
			{
				EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:otherNote", StringUtils::format("text:%s|site:{%f,%f}",a2u("无法捡起").c_str(),getPosition().x,getPosition().y));
				return false;
			}
			else
			{
				auto spar = dynamic_cast<Spar*>(Detect::shareDetect()->getCellByName(name));
				if(spar == nullptr) return false;
				addSpar(spar);
				spar->removeFromFloor();
			}
		}
		// 踩到机关
		else if((*data)["senderType"].asString() == "Switch")
		{
			log("I crashOn a Switch[%s]",(*data)["senderName"].asString().c_str());
			auto target = (*data)["senderName"].asString();
		}
	}
	else if((*data)["type"].asString() == "touchOn")
	{
		// 接触门
		if((*data)["senderType"].asString() == "Door")
		{
			auto keyIdValue = (*data)["effect"].asValueMap()["needKeyId"];
			auto target = (*data)["senderName"].asString();
			auto door = Detect::shareDetect()->getCellByName(target);
			if(!keyIdValue.isNull())
			{
				auto keyId = keyIdValue.asInt();
				if(_itemBag->getPropSizeByType(keyId) > 0)
				{
					// 钥匙开门
					EventHelper::getInstance()->dispatchByStr("target:"+target+"|type:keyOpen|senderName:"+getName(),"keyId:"+cocos2d::Value(keyId).asString());			
				}
				else if(!door->getModelByName("forceOpen").isNull())
				{
					// 强制开门
					EventHelper::getInstance()->dispatchByStr("target:"+target+"|type:forceOpen","");
				}
				else
				{
					log("I don't have a key[%d].",keyId);
					std::string effect;
					if(keyId == 0)
					{
						effect = "text:" + a2u("无法打开") + "|site:" + pointToString(getPosition());
					}
					else
					{
						auto key = Item::create(keyId);
						effect = "text:" + GETSTRING("none") + key->getNickName() + "|site:" + pointToString(getPosition());
					}

					EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:otherNote", effect);
				}
			}
			else
			{
				EventHelper::getInstance()->dispatchByStr("target:"+target+"|type:open|senderName:"+getName(),"");	
			}
		}
		// 摸到机关
		else if((*data)["senderType"].asString() == "Switch")
		{
			log("I touch a Switch[%s]",(*data)["senderName"].asString().c_str());
			auto target = (*data)["senderName"].asString();
		}
		// 碰到怪物
		else if((*data)["senderType"].asString() == "Monster" || (*data)["senderType"].asString() == "Role")
		{
			auto target = (*data)["senderName"].asString();
			auto cell = Detect::shareDetect()->getCellByName(target);
			auto actor = dynamic_cast<Actor*>(cell);
			int relation = cell->getModelByName("relation").asInt();
			if(relation == 1)
			{
				if(cell->getBusy() == false)
				{
					ChatManager::getInstance()->chatWith(target);
				}
			}
			else if(relation == 2)
			{
				if(cell->getBusy() == false)
				{
					auto swap = Swap::create(this->getName(), target);
					addChild(swap);
				}
			}
			else if(relation == 3)
			{
				if(cell->getBusy() == false && actor->getIsDead() == false)
				{
					_rival = (*data)["senderName"].asString();
					auto mhit = actor->getStr() - this->getDef();
					auto phit = this->getStr() - actor->getDef();
					mhit = mhit > 0 ? mhit:0;
					phit = phit > 0 ? phit:0;
					int mround = phit == 0 ? 10000000:ceil(actor->getHp() * 1.0 / phit);
					bool win = false;
					auto damage = mhit * (mround - 1);
					win = damage >= this->getHp() ? false : true;

					if(win == false)
					{
						PopPanel::getInstance()->confirm("canFight","type:Text---text:"+ a2u("实力不济，确定要去###type:Text---text:送死|color:{255,0,0}###type:Text---text:？"),PromptPanel::TYPE_DONE_CANCEL,[=](){
							auto fight = FightNew::create(this->getName(), _rival);
							addChild(fight);
						},nullptr,true,true);
					}
					else
					{
						auto fight = FightNew::create(this->getName(), _rival);
						addChild(fight);
					}
				}
			}
		}
	}
	else if((*data)["type"].asString() == "crashOut")
	{
		// 踩离机关
		if((*data)["senderType"].asString() == "Switch" && !effect["crashOut"].isNull())
		{
			log("I crashOut a Switch[%s]",(*data)["senderName"].asString().c_str());
			auto target = (*data)["senderName"].asString();
			EventHelper::getInstance()->dispatchByStr("target:"+target+"|type:"+effect["crashOut"].asString()+"|senderType:Role|senderName:"+getName()+"|senderTypeId:"+cocos2d::Value(getTypeId()).asString(),"");
		}
	}
	else if((*data)["type"].asString() == "touchOut")
	{

	}
	else if((*data)["type"].asString() == "useProp")
	{
		int typeId; 
		if(!(*data)["senderTypeId"].isNull())
			typeId = (*data)["senderTypeId"].asInt();
		else
			typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		log("I use a prop[%d].",typeId);
		usePropByType(typeId,num);
	}
	else if((*data)["type"].asString() == "touch")
	{
		if(_canGuide == false)
			return false;
		if(_floor != nullptr)
			_floor->touchUnit(this, _faceSite);
		//EventHelper::getInstance()->dispatchByStr("target:"+getFloor()->getName()+"|type:touchOn|senderType:Role|senderName:"+_model["name"].asString(),"faceSite:" + StringUtils::format("{%f,%f}",_faceSite.x,_faceSite.y));
	}
	else if((*data)["type"].asString() == "guideTo")
	{
		Vec2 site(0,0);
		if(!effect["site"].isNull())
		{
			site = PointFromString(effect["site"].asString());
		}
		else if(!effect["toCell"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(effect["toCell"].asString());
			if(cell != nullptr) site = cell->getSite();
		}
		else if(!effect["toCellTypeId"].isNull())
		{
			auto cell = getFloor()->getCellByType(effect["toCellTypeId"].asInt());
			if(cell != nullptr) site = cell->getSite();
		}
		guideTo(site);
	}
	else if((*data)["type"].asString() == "setGuide")
	{
		auto guide = effect["guide"].asInt();
		auto value = effect["value"].asInt();
		setGuide((Direction)guide,value);
	}
	else if((*data)["type"].asString() == "convey" && _conveyCount > 1)
	{
		int towerId = getFloor()->getTower()->getId();
		int floorId;
		if(!effect["towerId"].isNull())
		{
			towerId = effect["towerId"].asInt();
		}
		if(!effect["floorId"].isNull())
		{
			floorId = effect["floorId"].asInt();
		}
		else if(!effect["addFloorId"].isNull())
		{
			floorId = _floor->getId() + effect["addFloorId"].asInt();
		}
		else
		{
			floorId = _floor->getId();
		}
		if(floorId < 1 || floorId > Detect::shareDetect()->getTower(towerId)->getFloorsNum())
		{
			log("not valid floorId");
			return false;
		}
		_conveyCount = 0;

		Vec2 site(0,0);
		if(!effect["site"].isNull())
		{
			site = PointFromString(effect["site"].asString());
		}
		else if(!effect["toCell"].isNull())
		{
			auto cell = Detect::shareDetect()->getCellByName(effect["toCell"].asString());
			if(cell != nullptr) site = cell->getSite();
		}
		else if(!effect["toCellTypeId"].isNull())
		{
			auto cell = Detect::shareDetect()->getWorld()->getTower(towerId)->getFloor(floorId)->getCellByType(effect["toCellTypeId"].asInt());
			if(cell != nullptr) site = cell->getSite();
		}

		float delayTime = 0;
		if(!effect["delay"].isNull())
		{
			delayTime = effect["delay"].asFloat();
		}
		if(this->getType() == "Player")
			Detect::shareDetect()->setControlled(4,0);
		runAction(Sequence::create(DelayTime::create(delayTime),CallFunc::create([=](){
			convey(site,3,floorId,towerId);
			if(this->getType() == "Player")
				Detect::shareDetect()->setControlled(4,1);
		}),nullptr));
	}
	else if((*data)["type"].asString() == "chat")
	{
		log("I chat a role[%s]",(*data)["senderName"].asString().c_str());
		if(!effect["with"].isNull())
		{
			ChatManager::getInstance()->chatWith(effect["with"].asString());
		}
	}
	else if((*data)["type"].asString() == "setChatId")
	{
		auto id = effect["chatId"].asInt();
		_chatId = id;
	}
	// 战斗
	else if((*data)["type"].asString() == "fight")
	{
		auto rival = effect["with"].asString();
		auto fight = FightNew::create(this->getName(), rival);
		addChild(fight);
	}
	else if((*data)["type"].asString() == "stop")
	{
		_paths = nullptr;
		//_dis = Vec2(0,0);
		for(int i = 0; i < 5;i++)
		{
			_guides[i] = 0;
		}
	}
	else if((*data)["type"].asString() == "dead")
	{
		//Detect::getInstance()->removeCellByName(getName());
	}
	else if((*data)["type"].asString() == "addSpeed")
	{
		auto num = effect["num"].asInt();
		_saveSpeed += num;
		_speed += num;
	}
	else if((*data)["type"].asString() == "setSpeed")
	{
		auto num = effect["num"].asInt();
		_speed = num;
	}
	else if((*data)["type"].asString() == "resumeSpeed")
	{
		_speed -= _saveSpeed;
	}
	else if((*data)["type"].asString() == "setSite")
	{
		auto site = PointFromString(effect["site"].asString());
		setSite(site);
	}
	else if((*data)["type"].asString() == "addPropByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		addPropByType(typeId,num);
	}
	else if((*data)["type"].asString() == "removePropByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		removePropByType(typeId,num);
	}
	else if((*data)["type"].asString() == "addEquipByType")
	{
		auto typeId = effect["typeId"].asInt();
		addEquipByType(typeId);
	}
	else if((*data)["type"].asString() == "addSpecialByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		addSpecialByType(typeId,num);
	}
	else if((*data)["type"].asString() == "getGemByType")
	{
		auto typeId = effect["typeId"].asInt();
		auto num = effect["num"].asInt();
		getGemByType(typeId,num);
	}
	else if((*data)["type"].asString() == "setHp")
	{
		auto num = effect["num"].asInt();
		setHp(num);
	}
	else if((*data)["type"].asString() == "addHp")
	{
		auto num = effect["num"].asInt();
		addHp(num);
	}
	else if((*data)["type"].asString() == "addStr")
	{
		auto num = effect["num"].asInt();
		addStr(num);
	}
	else if((*data)["type"].asString() == "addDef")
	{
		auto num = effect["num"].asInt();
		addDef(num);
	}
	else if((*data)["type"].asString() == "setStr")
	{
		auto num = effect["num"].asInt();
		setStr(num);
	}
	else if((*data)["type"].asString() == "setDef")
	{
		auto num = effect["num"].asInt();
		setDef(num);
	}
	else if((*data)["type"].asString() == "setGold")
	{
		auto num = effect["num"].asInt();
		setGold(num);
	}
	else if((*data)["type"].asString() == "setXp")
	{
		auto num = effect["num"].asInt();
		setXp(num);
	}
	else if((*data)["type"].asString() == "addGold")
	{
		auto num = effect["num"].asInt();
		addGold(num);
	}
	else if((*data)["type"].asString() == "writeDiary")
	{
 		auto type = effect["type"].asString();
		auto content = effect["content"].asString();
		if(type == "tip")
		{
			_diary->addTip(content);
		}
		else if(type == "task")
		{
			_diary->addTask(content);
		}
		else if(type == "map")
		{
			_diary->addMap(content);
		}
	}
	else if((*data)["type"].asString() == "showDiary")
	{
		auto type = effect["type"].asString();
		auto content = effect["content"].asString();
		_diary->show(type,content);
		PopPanel::getInstance()->addPanel(_diary,1);
	}
	else if((*data)["type"].asString() == "disappear")
	{
		disappear();
	}
	return false;
}

void Role::getGemByType(int typeId, int num)
{
	Gem* gem;
	while(num--)
	{
		gem = Gem::create(typeId);

		switch (gem->getTypeId())
		{
		case 2003001:case 2003002:case 2003003:
			EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:use_drug");
			break;
		case 2003004:
			EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_gem_1");
			break;
		case 2003005:
			EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_gem_2");
			break;
			default:
				break;
		}

		auto effect = gem->getEffectMap();
		for(auto pair:effect)
		{
			if(pair.first == "str")
			{
				addStr(pair.second.asInt());
				log("str:%s=%d",pair.second.asString().c_str(),_str);
			}
			else if(pair.first == "def")
			{
				addDef(pair.second.asInt());
				log("def:%s=%d",pair.second.asString().c_str(),_def);
			}
			else if(pair.first == "xp")
			{
				addXp(pair.second.asInt());
				log("xp:%s=%d",pair.second.asString().c_str(),_xp);
			}
			else if(pair.first == "hp")
			{
				addHp(pair.second.asInt());
				log("hp:%s=%d",pair.second.asString().c_str(),_hp);
			}
			else if(pair.first == "gold")
			{
				addGold(pair.second.asInt());
				log("gold:%s=%d",pair.second.asString().c_str(),_hp);
			}
			else if(pair.first == "buff")
			{
				Detect::shareDetect()->getUser()->addBuff(Buff::create(pair.second.asInt()));
			}
		}
	}

	EventHelper::getInstance()->dispatchByStr("target:"+gem->getName()+"|type:used","");
}

void Role::usePropByType(int typeId, int num)
{
	Prop* prop;
	bool use = true;
	while(num--)
	{
		if(_itemBag->getPropSizeByType(typeId) == 0)
		{
			return;
		}
		prop = _itemBag->getPropByType(typeId);
		// 笔记撕页或藏宝图
		if(prop->getTypeId1() == 4 && prop->getType() == "Prop")
		{
			if(_itemBag->getSpecialByType(2007002) == nullptr)
			{
				PopPanel::getInstance()->note("treaMap","type:Text---text:"+ a2u("找到笔记本才可以查看"),1.0f,false,true);
				return;
			}
			auto type = prop->getModelByName("treaType").asString();
			auto name = "type:Text---text:" + prop->getModelByName("treaName").asString();
			std::string text = "type:Text---text:"+ a2u("恭喜获得");
			if(type == "tip")
			{
				_diary->addTip(name);
				text += a2u("重要情报");
			}
			else if(type == "task")
			{
				_diary->addTask(name);
				text += a2u("任务提示");
			}
			else if(type == "map")
			{
				_diary->addMap(name);
				text += a2u("藏宝图");
			}
			text += a2u(",请查看笔记本");
			if(!prop->getModelByName("treaText").isNull())
			{
				text = prop->getModelByName("treaText").asString();
			}
			PopPanel::getInstance()->confirm("treaMap",text,PromptPanel::TYPE_DONE,nullptr,nullptr,true,true);
		}
		// 锤头
		if(prop->getTypeId1() == 14 && prop->getType() == "Prop")
		{
			const Vec2 vecs[1] = {_faceSite - _site};
			const int typeIds[1] = {2005009};
			auto cell = _floor->getNearByTypeId(_site,vecs,1,typeIds,1);
			if(cell != nullptr && cell->getModelByName("notSmash").isNull())
			{
				auto sc = dynamic_cast<Switch*>(cell);
				sc->on();

				ValueMap value2;
				value2["typeId"] = 3001001;
				auto effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
				Detect::shareDetect()->addCell(effect2,cell->getSite(),4,_floor->getId(),_floor->getTower()->getId());
				effect2->erupt();
			}
			else
			{
				use = false;
				PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("无法使用锤头"));
			}
		}
		// 炸药包
		if(prop->getTypeId1() == 15 && prop->getType() == "Prop")
		{
			const Vec2 vecs[1] = {_faceSite - _site};
			const std::string types[2] = {"Monster","Role"};
			auto cell = _floor->getNearByType(_site,vecs,1,types,2);
			if(cell != nullptr)
			{
				if(cell->getModelByName("canBomb").asInt() == 1)
				{
					ValueMap value2;
					value2["typeId"] = 3001009;
					auto effect2 = static_cast<Effect*>(CellFactory::getInstance()->createCell(value2));
					effect2->setClearTime(1);
					BlendFunc func = {GL_ONE_MINUS_DST_COLOR, GL_ONE};
					effect2->getView()->setBlendFunc(func);
					Detect::shareDetect()->addCell(effect2,cell->getSite(),4,_floor->getId(),_floor->getTower()->getId());
					effect2->erupt();

					cell->disappear();

					_useBombNum++;
				}
				else
				{
					use = false;
					PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("炸药对他无效"));
				}
			}
			else
			{
				use = false;
				PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("无法炸到任何东西"));
			}
		}
		// 锄头
		else if(prop->getTypeId1() == 10)
		{
			auto cells = _floor->getUnit(_site)->getCells();
			bool canDig = false;
			Cell* locate = nullptr;
			cocos2d::Value fixTrea;
			for(auto cell:cells)
			{
				// 藏宝的地方
				if(cell->getTypeId() == 2005002)
				{
					canDig = true;
					locate = cell;
					if(!cell->getModelByName("fixTrea").isNull())
					{
						fixTrea = cell->getModelByName("fixTrea");
					}
					break;
				}
			}
			Point appearSite;
			// 地点
			std::vector<Point> points;	
			for(auto unit : CrossNoCenterVecs)
			{
				Point p(_site);
				p += unit;
				if(_blockFunc(p) == 0)
				{
					points.push_back(p);
				}
			}

			if(points.size() == 0)
			{
				canDig = false;
			}
			else
			{
				appearSite = points.at(CCRANDOM_0_1()*(points.size()-1));
			}

			if(canDig == true)
			{
				Cell* treasure = nullptr;
				if(!fixTrea.isNull())
				{
					treasure = Detect::shareDetect()->getCellByName(fixTrea.asString());
				}
				else
				{
					auto treaRates = prop->getEffectMap();
					/*auto doeRate = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "digRate","doe")).asFloat();
					auto locateRate = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "digRate","locate")).asFloat();*/
					ValueMap locateRates;
					if(!locate->getModelByName("rate").isNull())
					{
						stringToValueMap(locate->getModelByName("rate").asString(),locateRates);
					}

					for(auto pair1:treaRates)
					{
						for(auto pair2:locateRates)
						{
							if(pair1.first == pair2.first)
							{
								treaRates[pair1.first] = pair1.second.asInt() + pair2.second.asInt();
							}
						}
					}

					ValueVector treaTypes;
					ValueVector treaRateAreas;
					for(auto pair1:treaRates)
					{
						float rate = 0;
						if(treaRateAreas.size() > 0)
							rate = treaRateAreas.at(treaRateAreas.size()-1).asFloat();
						rate += pair1.second.asFloat();

						treaTypes.push_back(cocos2d::Value(pair1.first));
						treaRateAreas.push_back(cocos2d::Value(rate));
					}

					float random = CCRANDOM_0_1()*treaRateAreas.at(treaRateAreas.size()-1).asFloat();

					int i = 0;
					for(auto pair:treaRateAreas)
					{
						if(random <= pair.asFloat())
							break;
						i++;
					}
					int treasureType = treaTypes.at(i).asInt();
					ValueMap model;
					model["typeId"] = treasureType;
					model["name"] = "treasure_" + cocos2d::Value(cellCount++).asString();
					treasure = CellFactory::getInstance()->createCell(model);
					if(treasure != nullptr)
						treasure->retain();
				}

				//使用了一次锄头
				_useHoeNum++;
				auto findTreasure = true;
				if(treasure != nullptr)
				{
					if(Detect::shareDetect()->getCellByName(treasure->getName()) != nullptr)
					{
						runAction(Sequence::create(DelayTime::create(1.0),CallFunc::create([=](){Detect::shareDetect()->convey(treasure,appearSite,1,_floor->getId(),_floor->getTower()->getId());}),nullptr));
					}
					else
					{
						runAction(Sequence::create(DelayTime::create(1.0),CallFunc::create([=](){Detect::shareDetect()->addCell(treasure,appearSite,1,_floor->getId(),_floor->getTower()->getId());treasure->release();}),nullptr));
					}
					//Detect::shareDetect()->removeCell(locate);
				}
				else
				{
					PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("什么也没挖到"));
				}
			}
			else
			{
				use = false;
				PopPanel::getInstance()->note("cantDig","type:Text---text:"+ a2u("这个地方看来不能挖下去"));
			}
		}
		else
		{
			auto effect = prop->getEffectMap();
			for(auto pair:effect)
			{
				if(pair.first == "str")
				{
					addStr(pair.second.asInt());
					log("str:%s=%d",pair.second.asString().c_str(),_str);
				}
				else if(pair.first == "def")
				{
					addDef(pair.second.asInt());
					log("def:%s=%d",pair.second.asString().c_str(),_def);
				}
				else if(pair.first == "xp")
				{
					addXp(pair.second.asInt());
					log("xp:%s=%d",pair.second.asString().c_str(),_xp);
				}
				else if(pair.first == "hp")
				{
					addHp(pair.second.asInt());
					log("hp:%s=%d",pair.second.asString().c_str(),_hp);
				}
				else if(pair.first == "gold")
				{
					addGold(pair.second.asInt());
					log("gold:%s=%d",pair.second.asString().c_str(),_hp);
				}
				else if(pair.first == "doubleHp")
				{
					addHp(getHp());
					log("gold:%s=%d",pair.second.asString().c_str(),_hp);
				}
				else if(pair.first == "doubleSpeed")
				{
					_speed *= 2;
				}
				else if(pair.first == "halfFightDelay")
				{
					_fightDelay *= 0.5;
				}
			}
		}
		// 书信
		if(prop->getTypeId1() == 16 && prop->getType() == "Prop")
		{
			auto effect = prop->getEffectMap();
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
			use = false;
		}

		EventHelper::getInstance()->dispatchByStr("target:"+prop->getName()+"|type:used","");

		if(use == true)
		{
			prop->addDurable(-1);
			if(prop->getDurable() <= 0)
			{
				removeProp(prop);
			}
		}
	}	
}

void Role::convey(Vec2 site, int layerId, int floorId, int towerId)
{
	Detect::shareDetect()->convey(this, site, layerId, floorId, towerId);
}

// 走到。。附近
Point Role::getNearSite(Point core)
{
	std::vector<Point> points;	
	for(auto unit : CrossVecs)
	{
		Point p(core);
		p += unit;
		if(p == _site) return _site;
		if(_blockFunc(p) == 0)
		{
			points.push_back(p);
		}
	}
	int min = 10000000;
	int i = 0;
	int num;
	for(auto point:points)
	{
		int dis = abs(_site.x - point.x) + abs(_site.y - point.y);
		if(min > dis)
		{
			min = dis;
			num = i;
		}
		i++;
	}
	if(points.size() == 0)
		return _site;
	else
		return points.at(num);
}

void Role::guideTo(Vec2 site)
{
	if(_canGuide == false)
		return;
	int block = _blockFunc(site);
	if(block > 0)
	{
		// 转向
		Point p = site - _site;
		if(p == CrossVecs[1])
		{
			_guide = LEFT;
		}else if(p == CrossVecs[2])
		{
			_guide = UP;
		}else if(p == CrossVecs[3])
		{
			_guide = RIGHT;
		}else if(p == CrossVecs[4])
		{
			_guide = DOWN;
		}
		if(block == 2)
		{
			auto nearSite = getNearSite(site);
			if(nearSite != _site)
				moveTo(nearSite);
		}
		// 触发
		if(block > 0 && site == _faceSite)
		{
			if(_floor != nullptr)
				_floor->touchUnit(this, _faceSite);
			/*EventHelper::getInstance()->dispatchByStr("target:"+getFloor()->getName()+"|type:touchOn|senderType:Player|senderName:"+_model["name"].asString(),"faceSite:" + StringUtils::format("{%f,%f}",site.x,site.y));*/
		}
	}
	else
	{
		ValueMap map;
		map["typeId"] = 3001010;
		auto guideEffect = Effect::create(map);
		Detect::shareDetect()->addCellByType(3001010,site,2,_floor->getId(),_floor->getTower()->getId());
		moveTo(site);
	}
}

void Role::setGuide(Direction dir,int value)
{
	_guides[(int)dir] = value;
}

void Role::updateGuide()
{
	if(_canGuide == false)
		return;
	for(int guide = 0; guide < 5;guide++)
	{
		if(_guides[guide] == 1)
		{
			_guide = (Direction)guide;
			break;
		}
	}
}

void Role::dead()
{
	Actor::dead();
}

Player* Player::create(int typeId)
{
	Player *pRet = new Player(typeId);
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

Player* Player::create(ValueMap& model)
{
	Player *pRet = new Player(model);
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

void Player::initModel()
{
	Role::initModel();
}

void Player::loadModel(ValueMap &map)
{
	Role::loadModel(map);
}

ValueMap Player::saveModel()
{
	auto model = Role::saveModel();

	auto curTime = getTime();
	_durTime += difftime(curTime, _initTime);

	model["durTime"] = _durTime;
	return model;
}

void Player::initOthers()
{	
	Role::initOthers();
}

void Player::initNull()
{
	Role::initNull();
	_face = UP;
	_shader = Shader::create("shade.png", "sten.png");

	_fightDelay = cocos2d::Value(PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "fightNew","roundDelay")).asFloat();
}

void Player::refresh()
{
	Role::refresh();
	updateToHud();
}

void Player::onEnter()
{
	Role::onEnter();
}

ItemBag* Player::getItemBag()
{
	return _itemBag;
}

SparDisk* Player::getSparDisk()
{
	return _sparDisk;
}

DiaryPanel* Player::getDiary()
{
	return _diary;
}

void Player::update(float delta)
{
	Role::update(delta);	
}

void Player::setHp(int hp)
{
	Actor::setHp(hp);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setHp", StringUtils::format("num:%d|maxHp:%d",getHp(),_maxHp));
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}
void Player::setStr(int str)
{
	Actor::setStr(str);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setStr", StringUtils::format("num:%d",getStr()));
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}
void Player::setDef(int def)
{
	Actor::setDef(def);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setDef", StringUtils::format("num:%d",getDef()));
#if DETECT_ACTOR == 1
	Detect::shareDetect()->getCurFloor()->updateFight();
#endif
}
void Player::setXp(int xp)
{
	Actor::setXp(xp);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setXp", StringUtils::format("num:%d|lastXp:%d|nextXp:%d",getXp(),_lastXp,_nextXp));
}

void Player::setGold(int gold)
{
	Actor::setGold(gold);
	_itemBag->setGold(_gold);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setGold", StringUtils::format("num:%d",getGold()));
}

void Player::convey(Vec2 site, int layerId, int floorId, int towerId)
{
	Role::convey(site, layerId, floorId, towerId);
	if(getName() == Detect::shareDetect()->getPlayer()->getName())
	{
		Detect::shareDetect()->setCurFloor(floorId, towerId);
	}
}

void Player::setLevel(int level)
{
	auto oldLevel = _level;
	auto newLevel = level;

	if(oldLevel < 1)
	{
		oldLevel = 1;
	}
	
	int addHpNum = 0;
	int addStrNum = 0;
	int addDefNum = 0;
	auto i = oldLevel;
	for(i++; i <= newLevel; i++)
	{
		std::string rewardStr;
		if(DATA_MODE == 1)
		{
			rapidjson::Document &datas = Model::shareModel()->levelDatas;
			auto &data = Model::GetDataFromCol1(datas,i);
			rewardStr = DICHELPER->getStringValue_json(data,"reward");
		}
		else if(DATA_MODE == 2)
		{
			rewardStr = Model::shareModel()->levelCsv->getValueByIdAndName(i, "reward").asString();
		}

		ValueMap reward;
		stringToValueMap(rewardStr, reward);
		for(auto pair:reward)
		{
			if(pair.first == "str")
			{
				addStrNum += pair.second.asInt();
				addStr(pair.second.asInt());
				log("str:%s=%d",pair.second.asString().c_str(),_str);
			}
			else if(pair.first == "def")
			{
				addDefNum += pair.second.asInt();
				addDef(pair.second.asInt());
				log("def:%s=%d",pair.second.asString().c_str(),_def);
			}
			else if(pair.first == "xp")
			{
				addXp(pair.second.asInt());
				log("def:%s=%d",pair.second.asString().c_str(),_xp);
			}
			else if(pair.first == "hp")
			{
				addHpNum += pair.second.asInt();
				addHp(pair.second.asInt());
				log("hp:%s=%d",pair.second.asString().c_str(),_hp);
			}
		}
	}

	if(newLevel > oldLevel)
	{
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:uplevel");

		int nums[] = {addHpNum, addStrNum, addDefNum};
		auto panel = LevelUpPanel::create("levelUp");
		panel->initWithNums(nums);
		PopPanel::getInstance()->removeAllPanelsByOrder(1);
		PopPanel::getInstance()->addPanel(panel,1);
	}

	Actor::setLevel(level);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:setLevel", StringUtils::format("num:%d",_level));
}

void Player::addSpar(Spar *spar)
{
	Role::addSpar(spar);
	PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + spar->getNickName());
}

void Player::addEquip(Equip *equip)
{
	Role::addEquip(equip);
	PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + equip->getNickName());
	if(equip->getTypeId1() == 1)
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:sword");
	else if(equip->getTypeId1() == 2)
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:shield");
}

void Player::addSpecial(Special *special)
{
	Role::addSpecial(special);
	std::string note;
	if(special->getTypeId1() == 1)
	{
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName() + a2u(",开启晶石系统");
	}
	else if(special->getTypeId1() == 2)
	{
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName();
	}
	else if(special->getTypeId1() == 3)
	{
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName() + a2u(",开启传送功能");
	}
	else if(special->getTypeId1() == 4)
	{
		Detect::shareDetect()->lookActorOpen = true;
#if DETECT_ACTOR == 1
		Detect::shareDetect()->getCurFloor()->updateFight();
#endif
		note = "type:Text---text:"+ a2u("获得了 ") + special->getNickName() + a2u(",可以查看怪物数据");
	}
	PopPanel::getInstance()->note("getSpecial",note,1.5f);

	EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:special");
}

void Player::addProp(Prop *prop)
{
	Role::addProp(prop);
	auto typeId = prop->getTypeId();
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|propName:%s|site:{%f,%f}",typeId,1,_itemBag->getPropSizeByType(typeId),prop->getNickName().c_str(),getPosition().x,getPosition().y));
	switch (typeId)
	{
	case 2002001:case 2002002:case 2002003:
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_key");
		break;
	case 2002007:case 2002008:case 2002009:
		EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:pickup_gem_3");
		break;
	default:
		break;
	}
	// 笔记撕页
	if(prop->getTypeId1() == 4)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 晶石碎片
	else if(prop->getTypeId1() == 5)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 锄头
	else if(prop->getTypeId1() == 10)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 藏宝图
	else if(prop->getTypeId1() == 11)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 圣水
	else if(prop->getTypeId1() == 12)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 幸运金币
	else if(prop->getTypeId1() == 13)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 锤头
	else if(prop->getTypeId1() == 14)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 炸药包
	else if(prop->getTypeId1() == 15)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
	// 书信
	else if(prop->getTypeId1() == 16)
	{
		PopPanel::getInstance()->note("getSpar","type:Text---text:"+ a2u("获得了 ") + prop->getNickName());
	}
}

void Player::removeProp(Prop *prop)
{
	auto typeId = prop->getTypeId();
	auto nickName = prop->getNickName().c_str();
	Role::removeProp(prop);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:removeProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|propName:%s|site:{%f,%f}",typeId,1,_itemBag->getPropSizeByType(typeId),nickName,getPosition().x,getPosition().y));
}

void Player::addPropByType(int typeId, int num)
{
	Role::addPropByType(typeId,num);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|site:{%f,%f}",typeId,num,_itemBag->getPropSizeByType(typeId),getPosition().x,getPosition().y));
}

void Player::addSpecialByType(int typeId, int num)
{
	Role::addSpecialByType(typeId,num);
}

void Player::addEquipByType(int typeId)
{
	Role::addEquipByType(typeId);
}

void Player::addSparByType(int typeId)
{
	Role::addSparByType(typeId);
}

void Player::removePropByType(int typeId, int num)
{
	Role::removePropByType(typeId, num);
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:removeProp", StringUtils::format("propType:%d|addPropNum:%d|propNum:%d|site:{%f,%f}",typeId,num,_itemBag->getPropSizeByType(typeId),getPosition().x,getPosition().y));
}

bool Player::onTrigger(EventCustom *event)
{
	Role::onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	return false;
}

void Player::moveToNewSite()
{
	Role::moveToNewSite();
}

void Player::dead()
{
	conditions[getName()+"_dead"] = LuaValue::intValue(1);
	conditions[getName()+"_isDead"] = LuaValue::intValue(1);
	setIsDead(true);
	/*if(getModelByName("noDead").isNull())
	{
	PopPanel::getInstance()->addPanelByName("end",1);
	}*/
}

void Player::addHp(int hp)
{
	Role::addHp(hp);
	if(hp <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addHp", StringUtils::format("num:%d|site:{%f,%f}",hp,getPosition().x,getPosition().y));
}

void Player::addStr(int str)
{
	Role::addStr(str);
	if(str <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addStr", StringUtils::format("num:%d|site:{%f,%f}",str,getPosition().x,getPosition().y));
}

void Player::addDef(int def)
{
	Role::addDef(def);
	if(def <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addDef", StringUtils::format("num:%d|site:{%f,%f}",def,getPosition().x,getPosition().y));
}

void Player::addXp(int xp)
{
	Role::addXp(xp);
	if(xp <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addXp", StringUtils::format("num:%d|site:{%f,%f}",xp,getPosition().x,getPosition().y));
}

void Player::addGold(int gold)
{
	Role::addGold(gold);
	if(gold <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addGold", StringUtils::format("num:%d|site:{%f,%f}",gold,getPosition().x,getPosition().y));
}

void Player::addLevel(int level)
{
	Role::addLevel(level);
	if(level <= 0) return;
	EventHelper::getInstance()->dispatchByStr("target:hud|senderName:"+getName()+"|type:addLevel", StringUtils::format("num:%d|site:{%f,%f}",level,getPosition().x,getPosition().y));
}

void Player::updateToHud()
{
	setHp(_hp);
	setStr(_str);
	setDef(_def);
	setXp(_xp);
	setLevel(_level);
	setGold(_gold);

	addPropByType(2002001,0);
	addPropByType(2002002,0);
	addPropByType(2002003,0);
}
