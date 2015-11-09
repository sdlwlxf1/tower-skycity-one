#include "model/Model.h"
#include "utils/StringHandler.h"

USING_NS_CC;

static Model* model = nullptr;

Model::Model()
{
	if(DATA_MODE == 1)
	{
		// 初始化演员数据	
		getJsonFromFile("data/actor.json", actorDatas);
		getJsonFromFile("data/item.json", itemDatas);
		getJsonFromFile("data/effect.json", effectDatas);
		getJsonFromFile("data/level.json", levelDatas);
		getJsonFromFile("data/tower.json", towerDatas);
		getJsonFromFile("data/chat.json", chatDatas);
		getJsonFromFile("data/buff.json", buffDatas);
	}
	else if(DATA_MODE == 2)
	{
		actorCsv = new CSVParse();
		actorCsv->openFile("data/actor.txt");
		itemCsv = new CSVParse(); 
		itemCsv->openFile("data/item.txt");
		effectCsv = new CSVParse();
		effectCsv->openFile("data/effect.txt");
		levelCsv = new CSVParse();
		levelCsv->openFile("data/level.txt");
		towerCsv = new CSVParse();
		towerCsv->openFile("data/tower.txt");
		chatCsv = new CSVParse();
		chatCsv->openFile("data/chat.txt");
		buffCsv = new CSVParse();
		buffCsv->openFile("data/buff.txt");
	}
}

void Model::refresh()
{
	actorCsv = new CSVParse();
	actorCsv->openFile("data/actor.txt");
	itemCsv = new CSVParse(); 
	itemCsv->openFile("data/item.txt");
	effectCsv = new CSVParse();
	effectCsv->openFile("data/effect.txt");
	levelCsv = new CSVParse();
	levelCsv->openFile("data/level.txt");
	towerCsv = new CSVParse();
	towerCsv->openFile("data/tower.txt");
	chatCsv = new CSVParse();
	chatCsv->openFile("data/chat.txt");
	buffCsv = new CSVParse();
	buffCsv->openFile("data/buff.txt");
}

Model::~Model()
{

}

Model* Model::shareModel()
{
	if(model == nullptr)
	{
		model = new Model();
	}
	return model;
}

void Model::getJsonFromFile(const char* fileName, rapidjson::Document &jsonDict)
{
	unsigned char *pBytes = NULL;
	std::string jsonpath; 
	jsonpath = FileUtils::sharedFileUtils()->fullPathForFilename(fileName); 
	//unsigned long size = 0;
	ssize_t size = 0;
	pBytes = FileUtils::sharedFileUtils()->getFileData(jsonpath.c_str(), "r" , &size); 
	Data *data = new Data();
	data->copy(pBytes, size);
	std::string load_str = std::string((const char *)data->getBytes(), data->getSize() ); 
	CC_SAFE_DELETE(data); 
	jsonDict.Parse<0>(load_str.c_str());
}

void Model::JsonAddModel(rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> &data,ValueMap& model)
{
	for (auto member = data.MemberonBegin(); member < data.MemberonEnd(); ++member)
	{
		auto name = member->name.GetString();
		if(model[name].isNull())
		{
			if(member->value.IsInt())
			{
				model[name] = member->value.GetInt();
			}
			else if(member->value.IsString())
			{
				model[name] = member->value.GetString();
			}
			else if(member->value.IsBool())
			{
				model[name] = member->value.GetBool();
			}
			else if(member->value.IsDouble())
			{
				model[name] = member->value.GetDouble();
			}
		}
	}
}

void Model::modelAddModel(cocos2d::ValueMap& model1,cocos2d::ValueMap& model2)
{
	for (auto pair:model1)
	{
		auto name = pair.first;
		if(model2[name].isNull())
		{
			model2[name] = pair.second;
		}
	}
}

void Model::modelCoverModel(cocos2d::ValueMap& model1,cocos2d::ValueMap& model2)
{
	for (auto pair:model1)
	{
		auto name = pair.first;
		if(!model2[name].isNull())
		{
			model2[name] = pair.second;
		}
	}
}

GenericData& Model::GetDataFromCol1(Document &datas, int col1, int alike)
{
	for(int i = 0; i < datas.Size();++i)
	{
		auto& data = datas[i];
		auto member = data.MemberonBegin();
		if(member->value.IsInt())
		{
			if(alike > 0 && member->value.GetInt() > col1)
			{
				if(alike == 1)
				{
					return datas[i - 1];
				}
				else if(alike == 2)
				{
					return data;
				}
			}
			else if(alike == 0 && member->value.GetInt() == col1)
			{
				return data;
			}
		}
	}
	CCASSERT(true,"未找到数据");
}

GenericData& Model::GetDataFromCol1AndCol2(Document &datas, int col1, int col2, int alike)
{
	for(int i = 0; i < datas.Size();++i)
	{
		auto& data = datas[i];
		auto member1 = data.MemberonBegin();
		auto menber2 = member1 + 1;
		if(member1->value.IsInt())
		{
			if(alike > 0 && member1->value.GetInt() > col1 && menber2->value.GetInt() > col2)
			{
				if(alike == 1)
				{
					return datas[i - 1];
				}
				else if(alike == 2)
				{
					return data;
				}
			}
			else if(alike == 0 && member1->value.GetInt() == col1 && menber2->value.GetInt() == col2)
			{
				return data;
			}
		}
	}
	CCASSERT(true,"未找到数据");
}

GenericData& Model::GetDataFromCol2(Document &datas, int col2, int alike)
{
	for(int i = 0; i < datas.Size();++i)
	{
		auto& data = datas[i];
		auto member = data.MemberonBegin() + 1;
		if(member->value.IsInt())
		{
			if(alike > 0 && member->value.GetInt() > col2)
			{
				if(alike == 1)
				{
					return datas[i - 1];
				}
				else if(alike == 2)
				{
					return data;
				}
			}
			else if(alike == 0 && member->value.GetInt() == col2)
			{
				return data;
			}
		}
	}
	CCASSERT(true,"未找到数据");
}