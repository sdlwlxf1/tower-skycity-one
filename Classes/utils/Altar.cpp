#include "utils/Altar.h"
#include "utils/PlotScript.h"
#include "cell/CellFactory.h"
#include "Detect.h"

USING_NS_CC;

Altar::Altar()
{
	/*ValueMap modell;
	ValueVector formsl;
	int i = 0,j = 0,k = 0;
	for(int i = 0; i < 5; i++)
	{

		ValueMap forml;
		ValueVector rewardl;
		rewardl.push_back(cocos2d::Value("target:detect|type:add~~~typeId:2001001|site:{5,1}"));
		rewardl.push_back(cocos2d::Value("sdfsfsfsfsfsf"));
		ValueVector mapl;

		for(int j = 0; j < 7;j++)
		{
			ValueVector rowsl;
			for(int k = 0; k < 7; k++)
			{
				rowsl.push_back(cocos2d::Value(0));
			}
			mapl.push_back(cocos2d::Value(rowsl));
		}

		forml["map"] = mapl;
		forml["reward"] = rewardl;

		formsl.push_back(cocos2d::Value(forml));
	}
	modell["forms"] = formsl;
	ValueVector switchsl;
	for(int i = 2;i < 9; i++)
	{
		for(int j = 4; j < 11;j ++)
		{
			ValueMap smodel;
			smodel["typeId"] = 2005006;
			smodel["site"] = pointToString(Vec2(i,j));
			smodel["towerId"] = 6;
			smodel["floorId"] = 4;
			switchsl.push_back(cocos2d::Value(smodel));
		}
	}
	modell["switchs"] = switchsl;

	FileUtils::getInstance()->writeToFile(modell,"altar.plist");*/




	_model = FileUtils::getInstance()->getValueMapFromFile("altar.plist");
	forms = _model["forms"].asValueVector();

	int fromNum = 0;
	for(auto pair:forms)
	{
		std::vector<std::vector<int>> formTemp;

		auto form = pair.asValueMap();
		for(auto i:form["map"].asValueVector())
		{
			std::vector<int> rowTemp;
			for(auto j:i.asValueVector())
			{
				rowTemp.push_back(j.asInt());
			}
			formTemp.push_back(rowTemp);
		}
		maps.insert(std::make_pair(fromNum, formTemp));
		fromNum++;
	}

	

	ValueVector switchsm = _model["switchs"].asValueVector();
	for(auto pair:switchsm)
	{
		auto switchModel = pair.asValueMap();
		auto cell = CellFactory::getInstance()->createCell(switchModel);
		Detect::shareDetect()->addCell(cell,PointFromString(switchModel["site"].asString()),1,switchModel["floorId"].asInt(),switchModel["towerId"].asInt());
		_switchs.push_back(cell->getName());
	}

	//_listener = EventListenerCustom::create(getName(), CC_CALLBACK_1(World::onTrigger, this));
	_listener = EventListenerCustom::create("altar1", [=](EventCustom *event){onTrigger(event);});
	CC_SAFE_RETAIN(_listener);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
}

ValueMap Altar::saveModel()
{
	ValueMap model;
	model["removeIds"] = _removeIds;
	return model;
}

void Altar::loadModel(ValueMap &map)
{
	_removeIds = map["removeIds"].asValueVector();
	for(auto id:_removeIds)
	{
		maps.erase(id.asInt());
	}
}

void Altar::reset()
{
	_switchs.clear();
}

Altar::~Altar()
{

}

Altar* Altar::create()
{
	Altar *ret = new Altar();
	if (ret)
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

void Altar::check()
{
	bool fit = false;
	int fitId;
	for(auto pairI:maps)
	{
		if(checkMap(pairI.second) == true)
		{
			fit = true;
			fitId = pairI.first;
			break;
		}
	}

	if(fit == true)
	{
		auto formMap = forms.at(fitId).asValueMap();
		auto reward = formMap["reward"].asValueVector();
		for(auto pair:reward)
		{
			PlotScript::dispatchAll(pair.asString().c_str());
		}
		/*auto star_iter = std::find(forms.begin(),forms.end(),formMap);
		forms.erase(star_iter);*/
		maps.erase(fitId);

		_removeIds.push_back(cocos2d::Value(fitId));
	}

	for(auto name:_switchs)
	{
		auto cell = dynamic_cast<Switch*>(Detect::shareDetect()->getCellByName(name));
		cell->off();
	}
}

bool Altar::checkMap(std::vector<std::vector<int>> map)
{
	int i = 0,j = 0;
	for(auto pairI:map)
	{
		j = 0;
		for(auto pairJ:pairI)
		{
			auto name = _switchs.at(i * pairI.size() + j);
			auto cell = dynamic_cast<Switch*>(Detect::shareDetect()->getCellByName(name));
			if((int)cell->isOn() != pairJ)
			{
				return false;
			}
			j++;
		}
		i++;
	}
	return true;
}

const std::string Altar::getNameId()
{
	
	return _model["name"].asString();
}

bool Altar::onTrigger(EventCustom *event)
{
	//_curTower->onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	if((*data)["type"].asString() == "check")
	{
		check();
	}
	return false;
}

std::string Altar::getType()
{
	return "Altar";
}

void Altar::initModel()
{

}