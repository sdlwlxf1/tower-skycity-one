#include "Global.h"
#include "PlotScript.h"
#include "utils/StringHandler.h"
#include "utils/EventHelper.h"
#include "Detect.h"
#include "scene/GameScene.h"
#include "cell/CellFactory.h"


bool PlotScript::_isFirst;
PlotScript* PlotScript::_shared;

PlotScript* PlotScript::sharedHD(){
	if(!_isFirst){
		_shared = new PlotScript();
		_shared->delayCount = 0;
	}
	return _shared;
}

const char* PlotScript::getLuaVarString(const char* luaFileName,const char* varName){

	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	int isOpen = openLuaFile(ls, luaFileName);
	if(isOpen!=0){
		log("Can't open Lua Error: %i", isOpen);
		return NULL;
	}

	lua_settop(ls, 0);
	lua_getglobal(ls, varName);

	int statesCode = lua_isstring(ls, 1);
	if(statesCode!=1){
		log("Open Lua Error: %i", statesCode);
		return NULL;
	}

	const char* str = lua_tostring(ls, 1);
	lua_pop(ls, 1);

	return str;
}

int PlotScript::openLuaFile(lua_State* _ls, const char *_luaFile )
{
	int isOpen = -1;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string code("require \"");
	code.append(_luaFile);
	code.append("\"");
	//return executeString(code.c_str());
	isOpen = luaL_loadstring(_ls, code.c_str());
	lua_pcall(_ls, 0, LUA_MULTRET, 0);
	if(isOpen != 0)
	{
		log("Can't open Error: %s", _luaFile);
	}
#else
	if (luaL_loadfile(_ls, _luaFile) != 0) {
		log("loadfile error[file:%s]: %s", _luaFile,lua_tostring(_ls, -1));
	}

	if (lua_pcall(_ls, 0, LUA_MULTRET, 0) != 0) {
		log("pcall error[file: %s", _luaFile,lua_tostring(_ls, -1));
	}

	isOpen = luaL_dofile(_ls, _luaFile);
	if(isOpen != 0)
	{
		log("Open Lua Error: %s", _luaFile);
	}
#endif
	return isOpen;
}

const char* PlotScript::getLuaVarOneOfTable(const char* luaFileName,const char* varName,const char* keyName){

	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	int isOpen = openLuaFile(ls, luaFileName);
	if(isOpen!=0){
		log("Can't open Error: %i", isOpen);
		return NULL;
	}

	lua_getglobal(ls, varName);

	int statesCode = lua_istable(ls, -1);
	if(statesCode!=1){
		log("Open Lua Error: %i", statesCode);
		return NULL;
	}

	lua_pushstring(ls, keyName);
	lua_gettable(ls, -2);
	const char* valueString = lua_tostring(ls, -1);

	lua_pop(ls, -1);

	return valueString;
}

const char* PlotScript::getLuaVarTable(const char* luaFileName,const char* varName){
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	int isOpen = openLuaFile(ls, luaFileName);
	if(isOpen!=0){
		log("Can't open Error: %i", isOpen);
 		return NULL;
	}

	lua_getglobal(ls, varName);

	int statesCode = lua_istable(ls, -1);
	if(statesCode!=1){
		log("Open Lua Error: %i", statesCode);
		return NULL;
	}

	int it = lua_gettop(ls);
	lua_pushnil(ls);

	string result="";

	while(lua_next(ls, it))
	{
		string key = lua_tostring(ls, -2);
		string value = lua_tostring(ls, -1);

		result=result+key+":"+value+"\t";

		lua_pop(ls, 1);
	}
	lua_pop(ls, 1);

	return result.c_str();
}

const char* PlotScript::callLuaFunction(const char* luaFileName,const char* functionName){
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	int isOpen = openLuaFile(ls, luaFileName);
	if(isOpen!=0){
		log("Can't open Error: %i", isOpen);
		return NULL;
	}

	lua_getglobal(ls, functionName);

	LuaEngine::getInstance()->getLuaStack()->pushLuaValueDict(conditions);
	/*
	lua_call
	第一个参数:函数的参数个数
	第二个参数:函数返回值个数
	*/
	lua_call(ls, 1, 1);

	const char* iResult = lua_tostring(ls, -1);

	return iResult;
}

void  PlotScript::registerCppFunction(const char* luaFileName)
{

	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();
	/*
	Lua调用的C++的函数必须是静态的
	*/
	lua_register(ls, "SEND", dispatch);
	lua_register(ls, "COND", setCondition);
	lua_register(ls, "DELY", delayDispatch);
	lua_register(ls, "SET_CELL_MODEL", setCellModel);
	lua_register(ls, "ADD_CELL_WITH_TRIGGER", addCellWithTrigger);
}

int PlotScript::setCondition(lua_State* ls)
{
	const char *luaKey = lua_tostring(ls, 1);
	int luaValue = lua_tointeger(ls, 2);

	conditions[luaKey] = LuaValue::intValue(luaValue);

	lua_pushstring(ls, "OK");
	return 1;
}

int PlotScript::setCellModel(lua_State* ls)
{
	const char *cellName = lua_tostring(ls, 1);
	const char *modelName = lua_tostring(ls, 2);
	const char *modelValue = lua_tostring(ls, 3);

	auto detect = Detect::shareDetect();
	auto cell = detect->getCellByName(cellName);
	cell->setModelByName(modelName,cocos2d::Value(modelValue));

	lua_pushstring(ls, "OK");
	return 1;
}

int PlotScript::addCellWithTrigger(lua_State* ls)
{
	const char *cellName = lua_tostring(ls, 1);
	const char *cellType = lua_tostring(ls, 2);
	const char *trigger = lua_tostring(ls, 3);
	const char *effectStr = lua_tostring(ls, 4);

	ValueMap effect;
	stringToValueMap(effectStr, effect);

	ValueMap map;
	map["typeId"] = cellType;
	map["name"] = cellName;
	map["triggerL"] = trigger;
	auto cell = CellFactory::getInstance()->createCell(map);
	auto site = PointFromString(effect["site"].asString());
	int layerId = 0;
	if(!effect["layerId"].isNull())
		layerId = effect["layerId"].asInt();
	int floorId = Detect::shareDetect()->getCurFloor()->getId();
	if(!effect["floorId"].isNull())
		floorId = effect["floorId"].asInt();
	Detect::shareDetect()->addCell(cell,site,layerId,floorId);

	lua_pushstring(ls, "OK");
	return 1;
}

int PlotScript::delayDispatch(lua_State* ls)
{
	const char *luaStr = lua_tostring(ls, 1);
	float delay = lua_tointeger(ls, 2);
	const char *curDelName = lua_tostring(ls, 3);

	ValueVector *vec = new ValueVector();
	vec->push_back(cocos2d::Value(luaStr));
	vec->push_back(cocos2d::Value(curDelName));

	auto scene = Director::getInstance()->getRunningScene();

	auto action = Sequence::create(DelayTime::create(delay),CCCallFuncND::create(scene,callfuncND_selector(PlotScript::delayCallBack),(void*)vec),NULL);
	scene->runAction(action);
	PlotScript::sharedHD()->delayCount++;

	return 1;
}

void PlotScript::delayCallBack(Node* pTarget, void* data)
{
	auto vec = (ValueVector*)data;
	PlotScript::dispatchAll(vec->at(0).asString().c_str());
	conditions[vec->at(1).asString().c_str()] = LuaValue::intValue(1);
	CC_SAFE_DELETE(vec);

	PlotScript::sharedHD()->delayCount--;
}

void PlotScript::dispatchAll(const char* str)
{
	// 脚本初始化
	ValueVector triggerOn;
	ValueVector vec1;
	stringToValueVector(std::string(str),"###",vec1);
	for (auto i:vec1)
	{
		ValueVector vec2;
		stringToValueVector(i.asString(),"~~~",vec2);
		if(vec2.size() < 2)
		{
			vec2.push_back(cocos2d::Value(""));
		}
		triggerOn.push_back(cocos2d::Value(vec2));
	}

	for(auto trigger:triggerOn)
	{
		auto vec = trigger.asValueVector();
		ValueMap map;
		stringToValueMap(vec.at(0).asString(),map);
		if(map["target"].asString() == "player")
		{
			map["target"] = Detect::shareDetect()->getPlayer()->getName();
		}
		EventHelper::getInstance()->dispatchByStr(valueMapToString(map),vec.at(1).asString());
	}   
}

int PlotScript::dispatch(lua_State* ls)
{
	const char *luaStr = lua_tostring(ls, 1);

	dispatchAll(luaStr);
	//log("Lua调用cpp函数时传来的两个参数: %i  %s",luaNum,luaStr);
	/*
	返给Lua的值
	*/
	lua_pushstring(ls, "OK");
	/*
	返给Lua值个数
	*/
	return 1;
}

const char* PlotScript::getFileFullPath(const char* fileName){
	return FileUtils::sharedFileUtils()->fullPathForFilename(fileName).c_str();
}

PlotScript::~PlotScript(){

	CC_SAFE_DELETE(_shared);
	_shared=NULL;
}
