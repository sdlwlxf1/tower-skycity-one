#include "CSVParse.h"
#include "StringHandler.h"

using namespace cocos2d;

CSVParse::CSVParse(istream& fin, string sep)
{
	fieldsep = sep;
	cols = 0;

	intType.insert(std::make_pair("int",CSVParse::INT));
	intType.insert(std::make_pair("float",CSVParse::FLOAT));
	intType.insert(std::make_pair("string",CSVParse::STRING));
}

CSVParse::~CSVParse()
{
	
}

void CSVParse::StringSplit( const string& str, vector<string>& tokens, const char* delimiters )
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos-lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void CSVParse::split( vector<string>& field, string line )
{
	string fld;
	unsigned int i,j=0;

	if( line.length() ==0 )
		return;
	i=0;

	do 
	{
		if(j<line.length() && line[i]=='"')
			j = advquoted(line, fld, ++i);
		else
			j = advplain(line, fld, i);

		field.push_back(fld);
		i = j+1;
	} while (j<line.length());
}

int CSVParse::advplain( const string& s, string& fld, int i)
{
	unsigned int j;
	j = s.find_first_of(fieldsep, i);
	if(j>s.length())
		j=s.length();
	fld = string(s,i,j-i);
	return j;
}

int CSVParse::advquoted( const string& s, string& fld, int i)
{
	unsigned int j;
	fld = "";
	for (j=i; j<s.length(); ++j)
	{
		if(s[j]=='"' && s[++j]!='"')
		{
			unsigned int k = s.find_first_of(fieldsep, j);
			if(k>s.length())
				k = s.length();
			for(k-=j; k-->0;)
				fld += s[j++];
			break;
		}
		fld += s[j];
	}
	return j;
}

//解析 CVS 文件
bool CSVParse::openFile( const char* fileName )
{
	string pathKey = FileUtils::sharedFileUtils()->fullPathForFilename(fileName);
	unsigned char* pBuffer = nullptr;
	ssize_t bufferSize = 0;
	pBuffer = FileUtils::sharedFileUtils()->getFileData(pathKey.c_str(), "r", &bufferSize);

	string s = (char*)pBuffer;
	string str = s.substr(0,bufferSize);

	//str = a2u(str.c_str());

	vector<string> line;
	StringSplit(str, line, "\r\n");

	for(unsigned int i=0; i<line.size(); ++i)
	{
		vector<string> field;
		split(field, line[i]);
		data.push_back(field);
		cols = max(cols, (int)field.size());
	}
	parseData();
	return true;
}

void CSVParse::parseData()
{
	auto vecType = data.at(0);
	for(auto str:vecType)
	{
		vector<string> vec;
		StringSplit(str, vec, ":");
		_names.push_back(vec.at(0));
		_types.push_back(intType.at(vec.at(1)));
	}
	for(int i = 1; i < data.size(); i++)
	{
		ValueMap map;
		auto group = data.at(i);
		for(int j = 0; j < group.size(); j++)
		{
			auto str = group.at(j);
			Value value;
			switch(_types[j])
			{
			case INT:
				value = Value(str).asInt();
				break;
			case FLOAT:
				value = Value(str).asFloat();
				break;
			case STRING:
				value = Value(str).asString();
				break;
			}
			map[_names[j]] = value;
		}
		model[map[_names[0]].asInt()] = Value(map);
	}
}

//获取指定行列的数据
const char* CSVParse::getData(unsigned int rows, unsigned int cols )
{
	if (rows<0 || rows>=data.size() || cols<0 || cols>=data[rows].size())
	{
		return "";
	}
	return data[rows][cols].c_str();
}

//获取指定行列的数据
ValueMap CSVParse::getModelById(int typeId)
{
	return model[typeId].asValueMap();
}

//获取指定行列的数据
Value CSVParse::getValueByIdAndName(int typeId, string name)
{
	return model[typeId].asValueMap()[name];
}

//获取指定行列的数据
ValueMap CSVParse::getModelByAssignTypeAndValue(std::string type, Value value)
{
	for(auto pair:model)
	{
		auto map = pair.second.asValueMap();
		if(map[type] == value)
		{
			return map;
		}
	}
	return ValueMap();
}

//获取指定行列的数据
ValueMap CSVParse::getModelByAlikeTypeAndValue(std::string type, Value value)
{
	std::vector<Value> sortVec;
	for(auto pair:model)
	{
		sortVec.push_back(pair.second);
	}

	std::sort(sortVec.begin(),sortVec.end(),[=](const Value& t1,const Value& t2){
		return t1.asValueMap().at(type).asInt() < t2.asValueMap().at(type).asInt();
	});
	for(int i = 0; i < sortVec.size(); i++)
	{
		if(sortVec.at(i).asValueMap().at(type).asInt() > value.asInt())
		{
			return sortVec.at(i - 1).asValueMap();
		}
	}
	return ValueMap();
}

//获取指定数据的列下标
int CSVParse::findColsData( int cols, const char* value )
{
	for (unsigned int i=0; i<data.size(); ++i)
	{
		if(strcmp(getData(i,cols),value)==0)
			return i;
	}
	return -1;
}