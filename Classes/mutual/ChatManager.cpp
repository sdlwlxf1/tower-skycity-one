#include "ChatManager.h"
#include "Detect.h"

static ChatManager* ret = nullptr;

ChatManager* ChatManager::getInstance()
{
	if(ret == nullptr)
	{
		ret = new ChatManager();
		ret->autorelease();
		ret->retain();
	}
	return ret;
}

void ChatManager::reset()
{
	actorName = "";
	curChatId = 1;
	nextChatId = 1;
	over = 1;
	on = false;
	_actor = nullptr;
	wait = false;
	/*CC_SAFE_RELEASE(ret);
	ret == nullptr;*/
}

ChatManager::ChatManager()
{
	init();
}

bool ChatManager::init()
{
	initModel();
	return true;
}

void ChatManager::initModel()
{
	actorName = "";
	curChatId = 1;
	nextChatId = 1;
	over = 1;
	on = false;
	wait = false;
	_actor = nullptr;

	//_listener = EventListenerCustom::create("chatManager", CC_CALLBACK_1(ChatManager::onTrigger, this));
	_listener = EventListenerCustom::create("chatManager", [=](EventCustom *event){onTrigger(event);});
	CC_SAFE_RETAIN(_listener);
	_eventDispatcher->addEventListenerWithFixedPriority(_listener, 1);
}

void ChatManager::loadModel(ValueMap &map)
{
	auto curChatIdV = map["curChatId"];
	if(!curChatIdV.isNull())
	{
		curChatId = curChatIdV.asInt();
	}
	auto nextChatIdV = map["nextChatId"];
	if(!nextChatIdV.isNull())
	{
		nextChatId = nextChatIdV.asInt();
	}
	auto overV = map["over"];
	if(!overV.isNull())
	{
		over = overV.asInt();
	}
	auto onV = map["on"];
	if(!onV.isNull())
	{
		on = onV.asInt() == 1?true:false;
	}
	auto waitV = map["wait"];
	if(!onV.isNull())
	{
		wait = waitV.asInt() == 1?true:false;
	}
	auto actorNameV = map["actorName"];
	if(!actorNameV.isNull())
	{
		actorName = actorNameV.asString();
	}
	if(on == true)
	{
		setChat(curChatId);
	}
	else
	{
		_chatPanel->setEnabled(false);
		_actor = nullptr;
		actorName = "";
	}
}

ValueMap ChatManager::saveModel()
{
	ValueMap model;
	model["curChatId"] = curChatId;
	model["nextChatId"] = nextChatId;
	model["over"] = over;
	model["on"] = on ? 1:0;
	model["wait"] = wait ? 1:0;
	model["actorName"] = actorName;
	return model;
}

void ChatManager::setChatPanel(ChatPanel* chatPanel)
{
	_chatPanel = chatPanel; 
	_chatPanel->setEnabled(false);
}

bool ChatManager::onTrigger(EventCustom *event)
{
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	if((*data)["type"].asString() == "next")
	{
		nextChat();
	}
	else if((*data)["type"].asString() == "set")
	{
		auto id = effect["id"].asInt();
		setChat(id);
	}
	return false;
}

const std::string ChatManager::getNameId()
{
	return "chatManager";
}

std::string ChatManager::getType()
{
	return "ChatManager";
}

void ChatManager::nextChat()
{
	conditions["chatId_"+cocos2d::Value(curChatId).asString()+"_over"] = LuaValue::intValue(1);
	if(over == 1)
	{
		_chatPanel->setEnabled(false);

		Detect::shareDetect()->setControlled(1,true);
		_actor = nullptr;
		actorName = "";
		on = false;
		return;
	}
	if(wait == false)
	{
		setChat(nextChatId);
	}
}

void ChatManager::setChat(int id)
{
	if(id == 0)
		return;
	EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:nextChat");

	if(_chats.find(id) == _chats.end())
	{
		auto unit = ChatUnit::create(id);
		_chats.insert(id, unit);
	}
	auto chat = _chats.at(id);

	if(chat->task != "")
	{
		EventHelper::getInstance()->dispatchByStr("target:"+Detect::shareDetect()->getPlayer()->getName()+"|type:writeDiary",StringUtils::format("type:%s|content:%s","task",chat->task.c_str()));
	}

	if(chat->begin == 1)
	{
		Detect::shareDetect()->setControlled(1,false);
	}

	_chatPanel->setEnabled(false);
	auto delay = chat->delay;
	if(delay != 0)
	{
		wait = true;
		Detect::shareDetect()->runAction(Sequence::create(DelayTime::create(delay),CallFunc::create([=](){
			_chatPanel->setEnabled(true);
			_chatPanel->initWithChat(chat);
			wait = false;}),nullptr));
	}
	else
	{
		_chatPanel->setEnabled(true);
		_chatPanel->initWithChat(chat);
	}

	curChatId = id;
	nextChatId = chat->next;
	over = chat->over;
	conditions["chatId_"+cocos2d::Value(curChatId).asString()+"_begin"] = LuaValue::intValue(1);

	on = true;
	if(actorName != "")
	{
		_actor = static_cast<Role*>(Detect::shareDetect()->getCellByName(actorName));
		if(_actor == nullptr) _actor = static_cast<Role*>(Detect::shareDetect()->getCellFromWaitingRoomByName(actorName));
		if(_actor != nullptr)
			_actor->setChatId(curChatId);
	}	
}

void ChatManager::chatWith(std::string name)
{
	actorName = name;
	_actor = static_cast<Role*>(Detect::shareDetect()->getCellByName(actorName));
	if(_actor == nullptr) _actor = static_cast<Role*>(Detect::shareDetect()->getCellFromWaitingRoomByName(actorName));
	if(_actor == nullptr)
	{
		actorName = "";
		return;
	}

	setChat(_actor->getChatId());
}

//public function Chat(type:String, chatContentData:ChatContentData, tileSheetData:TileSheetData, npc:NPC = null, setContentId:int = -1) 
//{
//	this.type = type;
//	this.npc = npc;
//	this.chatContentData = chatContentData;
//	this.tileSheetData = tileSheetData;
//	this.setContentId = setContentId;		
//	Global.stage.addEventListener(KeyboardEvent.KEY_DOWN, keyDownListener, false, 0, true);
//	init();
//}
//
//private function init():void
//{
//	//从图片库中取出图片数据
//	roleChatImageBitmapDatas = tileSheetData.imageLibrary["roleChatImageBitmapDatas"];
//	chatBackground = tileSheetData.imageLibrary["chatBackground"];
//
//
//	if(type == "manual") {
//
//		switch(npc.data.name) {
//		case "术士":
//			speakerId = ChatContentData.SPEAKER_NPC1;					
//			break;
//		case "精灵":
//			speakerId = ChatContentData.SPEAKER_NPC2;
//			break;
//		case "公主":
//			speakerId = ChatContentData.SPEAKER_NPC3;
//			break;
//		case "魔女":
//			speakerId = ChatContentData.SPEAKER_NPC4;
//			break;
//		}
//		currContentId = chatContentData.npcStartContentIdArray[speakerId];
//
//	} else if (type == "auto") {
//		speakerId == ChatContentData.SPEAKER_PLAYER;
//		currContentId = chatContentData.npcStartContentIdArray[speakerId];
//	}
//	if (setContentId != -1) {
//		currContentId = setContentId;
//	}
//	currContent = chatContentData.chatContentArray[currContentId];
//	currSpeakerId = currContent[CHAT_SPEAKER];
//	if (currSpeakerId == ChatContentData.SPEAKER_OTHER) {
//		currRoleImage = null;
//	} else {
//
//		currRoleImageId = currContent[CHAT_IMAGE_ID];
//		currRoleImage = roleChatImageBitmapDatas[currSpeakerId][currRoleImageId];
//	}
//
//
//	setBackgroundBitmap(chatBackground);
//
//	var textFormat:TextFormat = new TextFormat("微软雅黑", 17,0xff000000, true,null, null,null,null,TextFormatAlign.LEFT);
//	createDisplayText(CHAT_CONTENT_TEXT, "", 180, 100, 25, textFormat);
//
//
//	displayTexts[CHAT_CONTENT_TEXT].defaultTextFormat = textFormat;			
//	displayTexts[CHAT_CONTENT_TEXT].wordWrap = true;
//	addImageBitmap(CHAT_SPEAKER_IMAGE, -130, -160 ,  currRoleImage);
//	changeDisplayText(CHAT_CONTENT_TEXT, currContent[CHAT_CONTENT]);
//	displayTexts[CHAT_CONTENT_TEXT].y = (backgroundBitmap.height - displayTexts[CHAT_CONTENT_TEXT].textHeight) * 0.5 - 10;
//}
//
//public function removeKeyListener():void {
//	Global.stage.removeEventListener(KeyboardEvent.KEY_DOWN, keyDownListener);
//}
//
//private function keyDownListener(e:KeyboardEvent):void 
//{
//	if (e.keyCode == 32) {
//
//		if (currContent[CHAT_OVER] == 1) {
//			if(setContentId == -1) {
//				chatContentData.npcStartContentIdArray[speakerId] = currContent[CHAT_NEXT];
//			}
//			Global.stage.removeEventListener(KeyboardEvent.KEY_DOWN, keyDownListener);
//			dispatchEvent(new ChatEvent(ChatEvent.CHAT_OVER, String(currContent[CHAT_ID])));
//		} else {
//			dispatchEvent(new CustomEventSound(CustomEventSound.PLAY_SOUND, AddId.SOUND_CHAT_CHANGE, false, 1, 0,0.3));
//		}
//
//		chatContentData.currContentId = currContent[CHAT_NEXT];
//		currContentId = chatContentData.currContentId;
//		currContent = chatContentData.chatContentArray[currContentId];
//		currSpeakerId = currContent[CHAT_SPEAKER];
//		if (currSpeakerId == ChatContentData.SPEAKER_OTHER) {
//			currRoleImage = null;
//		} else {
//			currRoleImageId = currContent[CHAT_IMAGE_ID];
//			currRoleImage = roleChatImageBitmapDatas[currSpeakerId][currRoleImageId];
//		}
//
//		changeDisplayText(CHAT_CONTENT_TEXT, currContent[CHAT_CONTENT]);
//		displayTexts[CHAT_CONTENT_TEXT].y = (backgroundBitmap.height - displayTexts[CHAT_CONTENT_TEXT].textHeight) * 0.5 - 10;
//
//		imageBitmaps[CHAT_SPEAKER_IMAGE].bitmapData = currRoleImage;
//
//
//
//
//	}
//
//
//}
