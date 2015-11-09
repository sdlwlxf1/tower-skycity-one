#ifndef __TOBJECT_H__
#define __TOBJECT_H__

#include "cocos2d.h"
#include "Global.h"
USING_NS_CC;

class TObject
{
public:
	virtual void initModel() = 0;
	virtual void loadModel(ValueMap &map) = 0;
	virtual ValueMap saveModel() = 0;
	virtual bool onTrigger(EventCustom *event) = 0;
	virtual const std::string getNameId() = 0;
	virtual std::string getType() = 0;
protected:
	ValueMap _model;
	EventListenerCustom *_listener;
};
#endif /*__TOBJECT_H__*/