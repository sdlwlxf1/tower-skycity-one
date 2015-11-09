#ifndef __SPARDISK_PANEL_H__
#define __SPARDISK_PANEL_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cell/item/SparDisk.h"
#include "SimplePanel.h"
#include "cocostudio/CCSGUIReader.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class SparBox:public Ref
{
public:
	static SparBox* create(Button* button,ImageView* bg,ImageView* image,TextAtlas* num);
	bool init(Button* button,ImageView* bg,ImageView* image,TextAtlas* num);
	void setNum(int n);
	int getNum(){return _number;}
	void setBg(std::string spriteFrameName){_bg->loadTexture(spriteFrameName, TextureResType::PLIST);}
	void setImage(std::string spriteFrameName){_image->loadTexture(spriteFrameName, TextureResType::PLIST);}
	void setFixSpar(int typeId = 0);
	int getFixSpar(){return _fixTypeId;}
	void setFocusd(bool value){_button->setHighlighted(value);}
	void setSpar(int typeId);
	int getSpar(){return _typeId;}
	CC_SYNTHESIZE(int, _tag, Tag);
private:
	Button* _button;
	ImageView* _bg;
	ImageView* _image;
	TextAtlas* _num;
	int _typeId;
	int _fixTypeId;
	int _number;
};

class SparDiskPanel : public SimplePanel
{	
public:
	enum buttons
	{
		EXIT = 1,
	};
	void onEnter();
	void onExit();
	SparDiskPanel(std::string name):SimplePanel(name){}
	static SparDiskPanel* create(std::string name);
	bool init();
private:
	SparDisk* _sparDisk;
	Layout* _equipSparPan;
	Layout* _bagSparPan;
	Layout* _introPan;
	Vector<SparBox*> _equipSparBoxs;
	Vector<SparBox*> _sparBoxs;
	SparBox* _selectSparBox;
	SparBox* _selectEquipSparBox;
	Text* _nickNameLab;
	Text* _introLab;
	Text* _patchNumLab;
	Text* _completeLab;
	Button* _refineBtn;
	int _introSpar;
private:
	void setSelectSparBox(SparBox* sparBox);
	void onRefine(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onEquipSparClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onSparClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void updateIntro();
	void updateSpars();
	void clearSpars();
};


#endif /*__SPARDISK_PANEL_H__*/