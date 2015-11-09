#ifndef __BUFF_TARGET_H__
#define __BUFF_TARGET_H__

#include "cocos2d.h"
#include "buff/Buff.h"
USING_NS_CC;

class BuffTarget
{
public:
	virtual void addBuff(Buff* buff) = 0;
	virtual void removeBuff(Buff* buff) = 0;
	virtual void updateBuffsEffect() = 0;
protected:
	// buff¡–±Ì
	Vector<Buff*> _buffs;
};
#endif /*__BUFF_TARGET_H__*/