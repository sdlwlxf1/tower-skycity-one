#ifndef __CHAT_MANAGER_H__
#define __CHAT_MANAGER_H__

#include "cocos2d.h"
#include "utils/Singleton.h"
#include "TObject.h"
#include "UI/ChatPanel.h"
#include "ChatUnit.h"
#include "cell/actor/Actor.h"
USING_NS_CC;

class ChatManager:public Node,public TObject
{
public:
	ChatManager();
	static ChatManager* getInstance();
	void reset();
	void nextChat();
	void setChat(int id);
	void chatWith(std::string name);

	bool init();
	void initModel();
	void loadModel(ValueMap &map);
	ValueMap saveModel();
	bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();
	void setChatPanel(ChatPanel* chatPanel);
public:
	int curChatId;
	int nextChatId;
	int over;
	bool on;
	bool wait;
	std::string actorName;
private:
	ChatPanel*  _chatPanel;
	Map<int, ChatUnit*> _chats;
	Actor* _actor;
};



#endif /*__CHAT_MANAGER_H__*/