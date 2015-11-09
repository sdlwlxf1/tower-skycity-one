#ifndef __EVENT_HELPER_H__
#define __EVENT_HELPER_H__

#include "cocos2d.h"
#include "Singleton.h"
#include "TObject.h"
#include "StringHandler.h"

USING_NS_CC;

class EventHelper : public Ref, public Singleton<EventHelper>
{
public:
	/*void dispatch(std::string type, std::string &target, TObject &me, ValueMap &effect = ValueMap())
	{
	ValueMap data;
	data["type"] = type;
	data["target"] = target;
	data["senderName"] = me.getNameId();
	data["senderType"] = me.getType();
	data["effect"] = effect;
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(target,&data);
	}*/
	//target:..|"type:..|senderType:..|senderName:..","effect:.."
	void dispatchByStr(std::string type,std::string effectStr)
	{
		ValueMap data;
		ValueMap effect;
		stringToValueMap(type, data);
		stringToValueMap(effectStr, effect);
		data["effect"] = effect;
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(data["target"].asString(),&data);
	}
};

#endif /*__EVENT_HELPER_H__*/
