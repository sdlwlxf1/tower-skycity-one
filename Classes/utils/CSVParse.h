#ifndef __CSV_PARSE_H__
#define __CSV_PARSE_H__

#include "cocos2d.h"
#include <iostream>
using namespace std;
USING_NS_CC;

class CSVParse
{
public:
	enum TYPE
	{
		INT,
		FLOAT,
		STRING
	};

	std::map<std::string, TYPE> intType;
	//CSVParse();
	~CSVParse();

	CSVParse(istream& fin = cin, string sep=",");

	//用以存储数据
	std::vector<std::vector<std::string>> data;

	ValueMapIntKey model;

private:
	string        fieldsep;
	int            cols;

	void StringSplit(const string& str, vector<string>& tokens, const char* delimiters);
	void split(vector<string>& field, string line);
	int advplain(const string& line, string& fld, int);
	int advquoted(const string& line, string& fld, int);

	void parseData();

public:
	bool openFile(const char* fileName);
	const char* getData(unsigned int rows, unsigned int cols);
	int findColsData(int cols, const char* value);

	ValueMap getModelById(int typeId);
	cocos2d::Value getValueByIdAndName(int typeId, string name);
	ValueMap getModelByAssignTypeAndValue(std::string type, cocos2d::Value value);
	ValueMap getModelByAlikeTypeAndValue(std::string type, cocos2d::Value value);

	inline int getCols(){return cols;}
	inline int getRows(){return data.size();};

private:
	std::vector<std::string> _names;
	std::vector<TYPE> _types;
};

#endif //__CSV_PARSE_H__