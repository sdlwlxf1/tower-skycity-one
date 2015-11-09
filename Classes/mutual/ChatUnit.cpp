#include "ChatUnit.h"
#include "model/Model.h"
#include "utils/StringHandler.h"

ChatUnit* ChatUnit::create(int id)
{
	ChatUnit *pRet = new ChatUnit(id);
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

ChatUnit* ChatUnit::create(ValueMap& model)
{
	ChatUnit *pRet = new ChatUnit(model);
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

ChatUnit::ChatUnit(ValueMap& model)
{
	_model = model;
	auto value = _model["id"];
	if(!value.isNull())
	{
		id = value.asInt();
	}
	else
	{
		id = 1;
	}
}

ChatUnit::ChatUnit(int typeId)
{
	id = typeId;
}


bool ChatUnit::init()
{
	initModel();
	return true;
}

void ChatUnit::loadModel(ValueMap &map)
{

}

void ChatUnit::initModel()
{
	int typeId = id;
	if(DATA_MODE == 1)
	{
		rapidjson::Document &datas = Model::shareModel()->chatDatas;
		auto &data = Model::GetDataFromCol1(datas,typeId);
		Model::JsonAddModel(data,_model);
	}
	else if(DATA_MODE == 2)
	{
		auto model = Model::shareModel()->chatCsv->getModelById(typeId);
		Model::modelAddModel(model,_model);
	}
	id = _model["id"].asInt();
	actorId = _model["actor"].asInt();
	next = _model["next"].asInt();
	begin = _model["begin"].asInt();
	over = _model["over"].asInt();
	imageId = _model["imageId"].asInt();
	content = _model["content"].asString();
	alignType = _model["alignType"].asString();
	task = _model["task"].asString();
	delay = _model["delay"].asFloat();
}

ValueMap ChatUnit::saveModel()
{
	return ValueMap();
}

bool ChatUnit::onTrigger(EventCustom *event)
{
	return false;
}

const std::string ChatUnit::getNameId()
{
	return "";
}

std::string ChatUnit::getType()
{
	return "ChatUnit";
}

