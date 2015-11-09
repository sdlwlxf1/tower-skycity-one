#include "Global.h"

int keyBoard[100];

LuaValueDict conditions;

const Vec2 CrossVecs[5] = {Vec2(0,0),Vec2(-1,0),Vec2(0,1),Vec2(1, 0),Vec2(0,-1)};

const Vec2 CrossNoCenterVecs[4] = {Vec2(-1,0),Vec2(0,1),Vec2(1, 0),Vec2(0,-1)};

const Vec2 SquareVecs[9] = {Vec2(-1,1),Vec2(-1,0),Vec2(-1,-1),Vec2(0,1),Vec2(0,0),Vec2(0,-1),Vec2(1,1),Vec2(1,0),Vec2(1,-1)};

const Vec2 SquareNoCenterVecs[8] = {Vec2(-1,1),Vec2(-1,0),Vec2(-1,-1),Vec2(0,1),Vec2(0,-1),Vec2(1,1),Vec2(1,0),Vec2(1,-1)};

int cellCount = 0;

Dictionary* textDic = nullptr;