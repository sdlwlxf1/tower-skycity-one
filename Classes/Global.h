#ifndef __GLOBAL_H__
#define __GLOBAL_H__


#define DETECT_ACTOR 1

#define DATA_MODE 2


#include "cocos2d.h"
#include "CCLuaEngine.h"

USING_NS_CC;

#define MAKE_PAIR(_VAL1_,_VAL2_) std::make_pair<std::string,Value>(Value(_VAL1_).asString(),Value(_VAL2_))

#define POSTOSITE(_POS_) Vec2(floor(_POS_.x*0.025),floor(_POS_.y*0.025))

#define GETSTRING(_STR_) std::string(((CCString*)textDic->objectForKey(_STR_))->getCString())

const int TILE_WIDTH = 40;
const int TILE_HEIGHT = 40;
const int TILE_HALF_WIDTH = 20;
const int TILE_HALF_HEIGHT = 20;
const int HORIZON_TILE_NUM = 11;
const int VERTICAL_TILE_NUM = 13;

const int CELL_WIDTH = 60;
const int CELL_HEIGHT = 80;
const int CELL_HALF_WIDTH = 30;
const int CELL_HALF_HEIGHT = 40;

extern LuaValueDict conditions;

// 十字
extern const Vec2 CrossVecs[5];//{Vec2(0,0),Vec2(-1,0),Vec2(0,1),Vec2(1, 0),Vec2(0,-1)}
// 十字无中点
extern const Vec2 CrossNoCenterVecs[4];
// 正方形
extern const Vec2 SquareVecs[9];//{Vec2(-1,1),Vec2(-1,0),Vec2(-1,-1),Vec2(0,1),Vec2(0,0),Vec2(0,-1),Vec2(1,1),Vec2(1,0),Vec2(1,-1)}

extern const Vec2 SquareNoCenterVecs[8];//{Vec2(-1,1),Vec2(-1,0),Vec2(-1,-1),Vec2(0,1),Vec2(0,-1),Vec2(1,1),Vec2(1,0),Vec2(1,-1)}

extern int keyBoard[100];

extern int cellCount;

extern Dictionary* textDic;

#endif /*__GLOBAL_H__*/