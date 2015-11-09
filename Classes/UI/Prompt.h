#ifndef __PROMPT_PANEL_H__
#define __PROMPT_PANEL_H__

#include "cocos2d.h"
#include "UI/SimplePanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class PromptPanel : public SimplePanel
{	
public:
	enum buttonType
	{
		TYPE_DONE = 1,
		TYPE_DONE_CANCEL,
		TYPE_NONE
	};
	enum buttons
	{
		DONE = 1,
		CANCEL
	};
	PromptPanel(std::string name):SimplePanel(name){}
	static PromptPanel* create(std::string name);
	bool init();
	void initWithContent(std::string contentStr, buttonType type);
	void setSelfDestroy(float delay = 1.0f);

	void setEnabled(bool bol);
	void setCenter();

	std::function<void()> onDone;
	std::function<void()> onCancel;
private:
	ImageView *_bg;
	Layout *_panelContent;
	RichText* _content;
	bool _enabled;
	void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
};


#endif /*__PROMPT_PANEL_H__*/