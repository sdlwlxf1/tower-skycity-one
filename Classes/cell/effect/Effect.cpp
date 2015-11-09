#include "Effect.h"
#include "model/Model.h"
#include "Detect.h"


std::string Effect::getType()
{
	return "Effect";
}

Effect* Effect::create(int id)
{
	Effect *pRet = new Effect(id);
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

Effect* Effect::create(ValueMap& model)
{
	Effect *pRet = new Effect(model);
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

Effect::Effect(ValueMap& model)
{
	_model = model;
	auto typeId = _model["typeId"];
	if(!typeId.isNull())
	{
		_typeId = typeId.asInt();
	}
	else
	{
		_typeId = 1;
	}
}

void Effect::initModel()
{
	Cell::initModel();
	// 初始化物品数据
	int typeId = _typeId;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->effectDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->effectCsv->getModelById(typeId);
		Model::modelAddModel(model,_model);
	}

	_animations = ValueMap();
	stringToValueMap(_model["animation"].asString(), _animations);
	_effect = ValueMap();
	stringToValueMap(_model["effect"].asString(), _effect);
	this->setName(_model["name"].asString());
	_isBlock = _model["isBlock"].asInt();
	_zorder = _model["zorder"].asInt();
	_retain = _model["retain"].asInt();
	_loop = false;
	_repeatTime = _model["repeatTime"].asInt();
	_clearTime = _model["clearTime"].asInt();
	if(_repeatTime >= 1)
	{
		_loop = true;
	}
	_time = 0;
	//setName(StringUtils::format("effect_%d",cellCount));
}

void Effect::setClearTime(int count)
{
	_clearTime = count;
}

void Effect::setRepeatTime(int count)
{
	_repeatTime = count;
}

ValueMap Effect::saveModel()
{
	return Cell::saveModel();
}

void Effect::loadModel(ValueMap &map)
{

}

int Effect::isBlock()
{
	return _isBlock;
}

void Effect::initOthers()
{
	Cell::initOthers();
}

int Effect::getTypeId1()
{
	return _model["typeId1"].asInt();
}

void Effect::initView()
{
	/*Cell::initView();
	setAnchorPoint(Vec2(0.5,0.211));
	this->setVisible(false);*/
	if(getTypeId1() == 1)
	{
		_view = Sprite::createWithSpriteFrameName(_model["image"].asString());
		_view->setAnchorPoint(Point(0.5,0.211));
		addChild(_view);
	}
	else if(getTypeId1() == 2)
	{
		runAction(Sequence::create(DelayTime::create(0.5), CallFunc::create([=](){
			ParticleSystemQuad* emitter = ParticleSystemQuad::create(_model["image"].asString());
			if(_model["duration"].asFloat() != 0)
			{
				emitter->setDuration(_model["duration"].asFloat());
			}
			if(_model["life"].asFloat() != 0)
			{
				emitter->setLife(_model["life"].asFloat());
			}
			emitter->setAnchorPoint(Point(0.5,0.211));
			//Vec2 postion = Vec2(0 * TILE_WIDTH + TILE_HALF_WIDTH,0 * TILE_HEIGHT + TILE_HALF_HEIGHT);
			emitter->setPosition(Vec2(0,0));

			addChild(emitter);

			emitter->setAutoRemoveOnFinish(true);

			if(_model["last"].asFloat() != 0)
			{
				runAction(Sequence::create(DelayTime::create(_model["last"].asFloat()), CallFunc::create([=](){
					removeFromFloor();
				}),nullptr));
			}
		}),nullptr));
	}
}

void Effect::onEnter()
{
	Cell::onEnter();
	if(_retain == 0)
		erupt();
}

void Effect::erupt()
{
	this->setVisible(true);
	setAnimation("erupt",_loop,CallFuncN::create([this](Ref* sender){
		this->setVisible(false);
		_time++;
		if(_time == _repeatTime)
		{
			this->stopAllActions();
		}
		if(_time == _clearTime)
		{
			removeFromFloor();
		}
	}),true);
}
