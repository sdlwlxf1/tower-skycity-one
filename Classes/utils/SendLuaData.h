//  SendLuaData.cpp
//
//  Lua and C++/C 交互类

#ifndef __PublicSendLuaData__
#define __PublicSendLuaData__

#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

class SendLuaData{
public:
    
    static SendLuaData* getInstance();
    
    /*
     直接获取Lua中得变量名值
     文件名 luaFileName
     变量名 varName
     */
    const char* getLuaVarString(const char* luaFileName,const char* varName);
    
    /*
     获取Lua中一个表中得节点名，甚至方法
     文件名luaFileName
     方法名varName
     节点名
     */
    const char* getLuaVarOneOfTable(const char* luaFileName,const char* varName,const char* keyName);
    
    /*
     调用Lua全局Table
     lua文件名luaFileName
     table变量名varName
     */
    const char* getLuaVarTable(const char* luaFileName,const char* varName);
    
    /*
     带参数有返回
     文件名luaFileName
     方法名functionName
     参数序列arraypar
     参数类型arraypartype
     */
    const char* callLuaFuncParReturn(const char* luaFileName,const char* functionName,CCArray* arraypar,CCArray* arraypartype);
    
    /*
     带参数无返回
     文件名luaFileName
     方法名functionName
     参数序列arraypar
     参数类型arraypartype
     */
    const void callLuaFuncPar(const char* luaFileName,const char* functionName,CCArray* arraypar,CCArray* arraypartype);
    
    
private:
    
    static bool _isFirst;
    static SendLuaData* m_instance;
    const char* getFileFullPath(const char* fileName);
    ~SendLuaData();
};

#endif