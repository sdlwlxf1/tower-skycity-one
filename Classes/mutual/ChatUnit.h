#ifndef __CHAT_UNIT_H__
#define __CHAT_UNIT_H__

#include "cocos2d.h"
#include "TObject.h"
USING_NS_CC;

class ChatUnit : public Ref, public TObject
{
public:
	ChatUnit(int id);
	ChatUnit(ValueMap& model);

	static ChatUnit* create(int id);
	static ChatUnit* create(ValueMap& model);

	bool init();
	void initModel();
	void loadModel(ValueMap &map);
	ValueMap saveModel();
	bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();
public:
	int id;
	int actorId;
	std::string content;
	int next;
	int over;
	int imageId;
	int begin;
	float delay;
	std::string task;
	std::string alignType;
};


#endif /*__CHAT_UNIT_H__*/