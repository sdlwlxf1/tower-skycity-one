#ifndef __ITEMBAG_PANEL_H__
#define __ITEMBAG_PANEL_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cell/item/ItemBag.h"
#include "SimplePanel.h"
#include "cocostudio/CCSGUIReader.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class ItemBox:public Ref
{
public:
	static ItemBox* create(Button* button,ImageView* bg,ImageView* image,TextAtlas* num);
	bool init(Button* button,ImageView* bg,ImageView* image,TextAtlas* num);
	void setNum(int n);
	int getNum(){return _number;}
	void setBg(std::string spriteFrameName){_bg->loadTexture(spriteFrameName, TextureResType::PLIST);}
	void setImage(std::string spriteFrameName){_image->loadTexture(spriteFrameName, TextureResType::PLIST);}
	void setFixItem(int typeId = 0);
	int getFixItem(){return _fixTypeId;}
	void setFocusd(bool value){_button->setHighlighted(value);}
	void setItem(int typeId);
	int getItem(){return _typeId;}
private:
	Button* _button;
	ImageView* _bg;
	ImageView* _image;
	TextAtlas* _num;
	int _typeId;
	int _fixTypeId;
	int _number;
};

class ItemBagPanel : public SimplePanel
{	
public:
	void onEnter();
	void onExit();
	ItemBagPanel(std::string name):SimplePanel(name){}
	static ItemBagPanel* create(std::string name);
	bool init();
private:
	ItemBag* _itemBag;
	Layout* _equipPan;
	Layout* _propPan;
	Layout* _specialPan;
	Layout* _introPan;
	Vector<ItemBox*> _equipBoxs;
	Vector<ItemBox*> _propBoxs;
	Vector<ItemBox*> _specialBoxs;
	ItemBox* _selectItemBox;
	Button* _closeBtn;
	Button* _useBtn;
	Text* _nickNameLab;
	Text* _introLab;
	Text* _durableLab;
private:
	void setSelectItemBox(ItemBox* itemBox);
	void onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void onUse(cocos2d::Ref *ref, Widget::TouchEventType touchType);
	void updateIntro();
	void updateItems();
	void clearItems();
};


#endif /*__ITEMBAG_PANEL_H__*/