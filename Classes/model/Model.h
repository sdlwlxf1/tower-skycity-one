#ifndef __MODEL_H__
#define __MODEL_H__

#include "Global.h"
#include "cocos2d.h"
#include "json/document.h"
#include "utils/CSVParse.h"

using namespace rapidjson;

typedef GenericValue<UTF8<char>, MemoryPoolAllocator<CrtAllocator>> GenericData;

#define DICHELPER cocostudio::DictionaryHelper::getInstance()

class Model : public cocos2d::Ref
{
public:
	Model();
	~Model();
	static Model* shareModel();
	static void getJsonFromFile(const char* fileName, Document &jsonDict);
	static void JsonAddModel(GenericData &data,cocos2d::ValueMap& model);
	static void modelAddModel(cocos2d::ValueMap& model1,cocos2d::ValueMap& model2);
	static void modelCoverModel(cocos2d::ValueMap& model1,cocos2d::ValueMap& model2);
	static GenericData& GetDataFromCol1(Document &datas, int col1, int alike = 0);
	static GenericData& GetDataFromCol2(Document &datas, int col2, int alike = 0);
	static GenericData& GetDataFromCol1AndCol2(Document &datas, int col1, int col2, int alike = 0);
	void refresh();
public:
	Document actorDatas;
	Document itemDatas;
	Document effectDatas;
	Document levelDatas;
	Document towerDatas;
	Document chatDatas;
	Document buffDatas;
public:
	CSVParse *actorCsv;
	CSVParse *itemCsv;
	CSVParse *effectCsv;
	CSVParse *levelCsv;
	CSVParse *towerCsv;
	CSVParse *chatCsv;
	CSVParse *buffCsv;
};

#endif /*__MODEL_H__*/