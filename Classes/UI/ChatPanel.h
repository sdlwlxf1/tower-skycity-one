#ifndef __CHAT_PANEL_H__
#define __CHAT_PANEL_H__

#include "UI/SimplePanel.h"
#include "mutual/ChatUnit.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class ChatPanel : public SimplePanel
{	
public:
	enum buttons
	{
		TOUCH = 1,
	};
	ChatPanel(std::string name):SimplePanel(name){}
	static ChatPanel* create(std::string name);
	void initWithChat(ChatUnit* chat);
	void setEnabled(bool bol);
	bool getEnabled(){return _enabled;}
	bool init();
private:
	ChatUnit *_chat;
	Layout *_layerHead;
	Button *_buttonTouch;
	Layout *_panelTouch;
	Layout *_panelContentLeft;
	Layout *_panelContentRight;
	Layout *_panelButtonLeft;
	Layout *_panelButtonRight;
	Layout *_panelImageLeft;
	Layout *_panelImageRight;
	Layout *_panelContentLarge;
	Layout *_buttonTouchLayout;
	Layout *_panelButtonTouch;
	RichText *_content;
	Action* _btnTouchAction;
	Sprite* _btnTouchSprite;

	ImageView* _headBg;
	ImageView* _headFace;
	bool _enabled;

	bool _startDelay;
private:
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__CHAT_PANEL_H__*/