#ifndef __HUD_PANEL_H__
#define __HUD_PANEL_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "UI/SimplePanel.h"
#include "UI/JoyStickPanel.h"
#include "UI/PopPanel.h"
#include "UI/BubbleLabel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;


class HudPanel : public SimplePanel , public TObject
{	
public:
	enum buttons
	{
		SHOP = 1,
		BAG,
		TRANS,
		BOOK,
		PAUSE
	};
	HudPanel(std::string name):SimplePanel(name){}
	static HudPanel* create(std::string name);
	bool init();
	~HudPanel();

	void initModel();
	void loadModel(ValueMap &map);
	ValueMap saveModel();
	bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();
	void onEnter();
	void onExit();

	void updateBuffList();
private:
	ListView* _buffList;
	Sequence* _stress1;
	bool _plotBlock;
	PopPanel* _popPanel;
	BubbleLabel* _bubbleLabel;
	LayerColor* _layerColor;
	Layout* _joyStickLayOut;
	JoyStickPanel* _joyStickPanel;
	virtual void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void selectedBuffEvent(Ref* pSender, ListView::EventType type);
};


#endif /*__HUD_PANEL_H__*/