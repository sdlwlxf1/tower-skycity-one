#include "User.h"
#include "Detect.h"
#include "utils/Tools.h"
#include "scene/GameScene.h"
#include "utils/PlotScript.h"
#include "ui/ShareAwardPanel.h"

User* User::create()
{
	User *pRet = new User();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool User::init()
{
	_multiSpeed = 1;
	_multiFightDelay = 1;

	_path = FileUtils::sharedFileUtils()->getWritablePath() + "user.plist";
	if(FileUtils::getInstance()->isFileExist(_path) == false)
	{
		auto model = saveModel();
		FileUtils::getInstance()->writeToFile(model,_path);
	}
	loadModel(FileUtils::getInstance()->getValueMapFromFile(_path));

	//reset();
	return true;
}

void User::pushAchieve(ValueMap model)
{
	_achieves.push_back(model);

	ValueMap map = saveModel();
	FileUtils::getInstance()->writeToFile(map,_path);
}

void User::removeAchieve(int index)
{
	_achieves.erase(_achieves.begin() + index);
}

ValueMap User::getAchieve(int index)
{
	return _achieves.at(index);
}

ValueMap User::getLastAchieve()
{
	return _achieves.at(_achieves.size() - 1);
}

std::vector<ValueMap> User::getAchieves()
{
	return _achieves;
}

void User::setShareRewardOpen(bool value)
{
	_shareRewardOpen = value;
	if(_shareRewardOpen == true && _shareRewardOpenFirst == true)
	{
		PopPanel::getInstance()->addPanel(ShareAwardPanel::create("shareAward"),1);
		_shareRewardOpenFirst = false;
	}

	// 保存
	ValueMap map = saveModel();
	FileUtils::getInstance()->writeToFile(map,_path);
}

void User::setShareRewardsLock(int index, int value)
{
	_shareRewardsLock[index] = value;

	auto node = PopPanel::getInstance()->hasPanelByName("shareAward");
	if(node != nullptr)
	{
		auto shareAwardPanel = dynamic_cast<ShareAwardPanel*>(node);
		shareAwardPanel->updateIntro();
	}

	// 坚持是否都获得
	bool open = false;
	for(int i = 0; i < 2; i++)
	{
		if(_shareRewardsLock[i] == 0)
		{
			open = true;
			break;
		}
	}
	if(open == false)
		setShareRewardOpen(false);

	// 保存
	ValueMap map = saveModel();
	FileUtils::getInstance()->writeToFile(map,_path);
}

void User::getShareReward(int index)
{
	if(getShareRewardsLock(index - 1) == 1)
	{
		PopPanel::getInstance()->note("shareReward","type:Text---text:"+ a2u("已经获得该奖励"),1.0f,false,true);
		return;
	}
	
	auto name = "shareReward" + cocos2d::Value(index).asString();
	
	ValueVector rewards;
	ValueVector rewardVec;
	stringToValueVector(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", name.c_str()),"###",rewardVec);
	for(auto pair:rewardVec)
	{
		ValueMap rewardMap;
		stringToValueMap(pair.asString(), rewardMap);
		rewards.push_back(cocos2d::Value(rewardMap));
	}

	// 默认只有一个奖品
	auto rewardMap = rewards.at(0).asValueMap();
	int typeId = rewardMap["typeId"].asInt();
	int num = rewardMap["num"].asInt();

	for(int i = 0; i < num; i ++)
	{
		auto prop = Prop::create(typeId);
		// 获得奖励
		auto player = Detect::shareDetect()->getPlayer();
		player->addProp(prop);
	}

	auto prop = Prop::create(typeId);
	PopPanel::getInstance()->note("cantEquip","type:Text---text:"+ a2u("恭喜获得 ###") + "type:Text---text:"+ prop->getNickName() + "|color:{255,0,0}");

	setShareRewardsLock(index - 1, 1);
}

ValueMap User::saveModel()
{
	ValueMap model;

	ValueVector achieves;
	for(auto pair:_achieves)
	{
		achieves.push_back(cocos2d::Value(pair));
	}
	model["achieves"] = achieves;

	ValueVector buffs;
	for(auto pair:_buffs)
	{
		buffs.push_back(cocos2d::Value(pair->saveModel()));
	}
	model["buffs"] = buffs;

	ValueVector shareRewardsLock;
	for(int i = 0; i < 3; i++)
	{
		shareRewardsLock.push_back(cocos2d::Value(_shareRewardsLock[i]));
	}
	model["shareRewardsLock"] = shareRewardsLock;

	model["firstOpen"] = int(_firstOpen);

	model["shareRewardOpen"] = int(_shareRewardOpen);

	model["shareRewardOpenFirst"] = int(_shareRewardOpenFirst);
	return model;
	
}

void User::reset()
{
	_achieves.clear();
	_buffs.clear();

	_firstOpen = true;
	for(int i = 0; i < 3; i++)
	{
		_shareRewardsLock[i] = 0;
	}
	_shareRewardOpen = false;
	_shareRewardOpenFirst = true;

	ValueMap map = saveModel();
	FileUtils::getInstance()->writeToFile(map,_path);
}

void User::loadModel(ValueMap model)
{
	_model = model;

	_achieves.clear();
	ValueVector achieves = model["achieves"].asValueVector();
	for(auto pair:achieves)
	{
		_achieves.push_back(pair.asValueMap());
	}

	_buffs.clear();
	ValueVector buffs = model["buffs"].asValueVector();
	for(auto pair:buffs)
	{
		auto buff = Buff::create(pair.asValueMap());
		addBuff(buff);
	}

	_firstOpen = true;
	if(!_model["firstOpen"].isNull())
	{
		_firstOpen = (bool)_model["firstOpen"].asInt();
	}

	for(int i = 0; i < 3; i++)
	{
		_shareRewardsLock[i] = 0;
	}
	if(!_model["shareRewardsLock"].isNull())
	{
		ValueVector shareRewardsLock = _model["shareRewardsLock"].asValueVector();
		for(int i = 0; i < 3; i++)
		{
			_shareRewardsLock[i] = shareRewardsLock.at(i).asInt();
		}
	}

	_shareRewardOpen = false;
	if(!_model["shareRewardOpen"].isNull())
	{
		_shareRewardOpen = (bool)_model["shareRewardOpen"].asInt();
	}

	_shareRewardOpenFirst = true;
	if(!_model["shareRewardOpenFirst"].isNull())
	{
		_shareRewardOpenFirst = (bool)_model["shareRewardOpenFirst"].asInt();
	}
}

void User::addBuff(Buff* buff)
{
	buff->setTarget(this);
	_buffs.pushBack(buff);
	updateBuffsEffect();

	/*PopPanel::getInstance()->note("treaMap","type:Text---text:"+ a2u("获得 ") + buff->getModelByName("nickName").asString(),1.0f,false,true);*/

	ValueMap map = saveModel();
	FileUtils::getInstance()->writeToFile(map,_path);
}


Buff* User::getBuffByTypeId(int typeId)
{
	for(auto pair:_buffs)
	{
		if(pair->getTypeId() == typeId)
			return pair;
	}
	return nullptr;
}

void User::removeBuff(Buff* buff)
{
	buff->setTarget(nullptr);
	_buffs.eraseObject(buff);
	updateBuffsEffect();

	ValueMap map = saveModel();
	FileUtils::getInstance()->writeToFile(map,_path);
}

void User::updateBuffsEffect()
{
	setMultiFightDelay(1);
	setMultiSpeed(1);
	for(auto buff:_buffs)
	{
		auto effectMap = buff->getEffectMap();
		for(auto pair:effectMap)
		{
			auto type = pair.first;
			auto value = pair.second;
			if(type == "moveSpeed")
			{
				setMultiSpeed(value.asFloat());
			}
			else if(type == "fightDelay")
			{
				setMultiFightDelay(value.asFloat());
			}
		}
	}

	auto gameScene = dynamic_cast<GameScene*>(Director::sharedDirector()->getRunningScene());
	if(gameScene != nullptr)
	{
		auto hud = gameScene->getHud();
		hud->updateBuffList();
	}
}

void User::setModelByName(std::string name, cocos2d::Value value)
{
	_model[name] = value;
}

cocos2d::Value User::getModelByName(std::string name)
{
	return _model[name];
}

User::~User()
{
	//ValueMap map = saveModel();
	//FileUtils::getInstance()->writeToFile(map,_path);
}