#include "ItemBagPanel.h"
#include "utils/StringHandler.h"
#include "model/Model.h"
#include "PopPanel.h"
#include "Detect.h"
#include "ui/SparDiskPanel.h"
#include "mutual/Swap.h"
#include "cell/CellFactory.h"

#define NONE_IMAGE "none.png"

ItemBox* ItemBox::create(Button* button,ImageView* bg,ImageView* image,TextAtlas* num)
{
	ItemBox *pRet = new ItemBox();
	if (pRet && pRet->init(button,bg,image,num))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool ItemBox::init(Button* button,ImageView* bg,ImageView* image,TextAtlas* num)
{
	button->setUserObject(this);
	_image = image;
	_bg = bg;
	_button = button;
	_num = num;

	_image->setEnabled(false);
	_num->setEnabled(false);
	_bg->setEnabled(false);
	setNum(0);
	// 音效
	EventHelper::getInstance()->dispatchByStr("target:detect|type:playEffect","name:itemBagOpen");

	return true;
}

void ItemBox::setFixItem(int typeId)
{
	if(typeId <= 0)
		return;
	/*rapidjson::Document &datas = Model::shareModel()->itemDatas;
	auto &data = Model::GetDataFromCol1(datas,typeId);
	auto bg_image = DICHELPER->getStringValue_json(data,"image:string");
	auto typeId1 = DICHELPER->getIntValue_json(data,"typeId1:int");*/

	auto item = Item::create(typeId);
	
	//item->setPosition(TILE_HALF_WIDTH, TILE_HALF_HEIGHT);
	auto view = item->getView();
	view->retain();
	view->removeFromParent();
	view->setAnchorPoint(Vec2(0.16,0));
	_bg->addChild(view);
	view->release();

	_bg->setCascadeColorEnabled(true);
	_bg->setColor(Color3B::BLACK);
	_bg->setOpacity(100);
	//setBg(bg_image);
	_fixTypeId = item->getTypeId1();
}

void ItemBox::setItem(int typeId)
{
	_typeId = typeId;
	if(typeId <= 0)
	{
		_bg->setOpacity(100);
		setImage(NONE_IMAGE);
		_image->removeAllChildren();
		return;
	}
	_bg->setOpacity(0);
	auto item = Item::create(typeId);
	auto view = item->getView();
	view->retain();
	view->removeFromParent();
	view->setAnchorPoint(Vec2(0.16,0));
	_image->addChild(view);
	view->release();
}

void ItemBox::setNum(int n)
{
	_number = n;
	if(n > 1)
	{
		_num->setString(cocos2d::Value(n).asString());
	}
	else
	{
		_num->setString("");
		if(n == 0)
		{
			setItem(0);
		}
	}
}

ItemBagPanel* ItemBagPanel::create(std::string name)
{
	ItemBagPanel *pRet = new ItemBagPanel(name);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void ItemBagPanel::onEnter()
{
	Layer::onEnter();
}

void ItemBagPanel::onExit()
{
	Layer::onExit();
}

bool ItemBagPanel::init()
{
	_itemBag = Detect::shareDetect()->getPlayer()->getItemBag();

	_equipPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_equip"));
	_propPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_prop"));
	_specialPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_special"));
	_introPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_intro"));
	_closeBtn = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_close"));
	_useBtn = static_cast<Button*>(_introPan->getChildByName("Button_use"));
	_nickNameLab = static_cast<Text*>(_introPan->getChildByName("Label_name"));
	_introLab = static_cast<Text*>(_introPan->getChildByName("Label_intro"));
	_durableLab = static_cast<Text*>(_introPan->getChildByName("Label_durable"));


	_useBtn->addTouchEventListener(CC_CALLBACK_2(ItemBagPanel::onUse, this));
	_closeBtn->addTouchEventListener(CC_CALLBACK_2(ItemBagPanel::onClose, this));

	// 初始化阴影
	// equips
	for(auto itemPan:_equipPan->getChildren())
	{
		auto image = static_cast<ImageView*>(itemPan->getChildByName("Image_item"));
		auto bg = static_cast<ImageView*>(itemPan->getChildByName("Image_bg_item"));
		auto button = static_cast<Button*>(itemPan->getChildByName("Button_item"));
		auto num = static_cast<TextAtlas*>(itemPan->getChildByName("AtlasLabel"));
		auto itemBox = ItemBox::create(button,bg,image,num);
		_equipBoxs.pushBack(itemBox);
		button->addTouchEventListener(CC_CALLBACK_2(ItemBagPanel::onButtonClicked, this));

		auto name = itemPan->getName();
		ValueVector valueName;
		stringToValueVector(name, "_", valueName);
		auto typeId = valueName.at(valueName.size() - 1).asInt();
		itemBox->setFixItem(typeId);
	}

	// props
	for(auto itemPan:_propPan->getChildren())
	{
		auto image = static_cast<ImageView*>(itemPan->getChildByName("Image_item"));
		auto bg = static_cast<ImageView*>(itemPan->getChildByName("Image_bg_item"));
		auto button = static_cast<Button*>(itemPan->getChildByName("Button_item"));
		auto num = static_cast<TextAtlas*>(itemPan->getChildByName("AtlasLabel"));
		auto itemBox = ItemBox::create(button,bg,image,num);
		_propBoxs.pushBack(itemBox);
		button->addTouchEventListener(CC_CALLBACK_2(ItemBagPanel::onButtonClicked, this));
	}

	// special
	for(auto itemPan:_specialPan->getChildren())
	{
		auto image = static_cast<ImageView*>(itemPan->getChildByName("Image_item"));
		auto bg = static_cast<ImageView*>(itemPan->getChildByName("Image_bg_item"));
		auto button = static_cast<Button*>(itemPan->getChildByName("Button_item"));
		auto num = static_cast<TextAtlas*>(itemPan->getChildByName("AtlasLabel"));
		auto itemBox = ItemBox::create(button,bg,image,num);
		_specialBoxs.pushBack(itemBox);
		button->addTouchEventListener(CC_CALLBACK_2(ItemBagPanel::onButtonClicked, this));

		/*auto name = itemPan->getName();
		ValueVector valueName;
		stringToValueVector(name, "_", valueName);
		auto typeId = valueName.at(valueName.size() - 1).asInt();
		itemBox->setFixItem(typeId);*/
	}
	_selectItemBox = nullptr;
	updateItems();
	updateIntro();
	return true;
}

void ItemBagPanel::clearItems()
{
	for(auto equipBox:_equipBoxs)
	{
		equipBox->setNum(0);
	}
	for(auto propBox:_propBoxs)
	{
		propBox->setNum(0);
	}
	for(auto specialBox:_specialBoxs)
	{
		specialBox->setNum(0);
	}
}

void ItemBagPanel::updateItems()
{
	clearItems();
	Map<int, Equip*>& equips = _itemBag->getEquips();
	std::map<int, Vector<Prop*>>& props = _itemBag->getProps();
	std::map<int, Vector<Special*>>& specials = _itemBag->getSpecials();

	for(auto equip:equips)
	{
		for(auto equipBox:_equipBoxs)
		{
			if(equip.first == equipBox->getFixItem())
			{
				equipBox->setItem(equip.second->getTypeId());
				break;
			}
		}
	}

	int i = 0;
	for(auto prop:props)
	{
		auto propBox = _propBoxs.at(i);
		propBox->setNum(prop.second.size());
		if(prop.second.size() <= 0)
		{
			propBox->setItem(0);
		}
		else
		{
			propBox->setItem(prop.second.at(0)->getTypeId());
		}
		i++;
	}

	i = 0;
	for(auto special:specials)
	{
		auto specialBox = _specialBoxs.at(i);
		specialBox->setNum(special.second.size());
		if(special.second.size() <= 0)
		{
			specialBox->setItem(0);
		}
		else 
		{
			specialBox->setItem(special.second.at(0)->getTypeId());
		}
		i++;
	}
}

void ItemBagPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) {
		Button *button = (Button*)ref;
		auto itemBox = static_cast<ItemBox*>(button->getUserObject());
		setSelectItemBox(itemBox);
	}
}

void ItemBagPanel::setSelectItemBox(ItemBox* itemBox)
{
	if(itemBox->getItem() == 0)
	{
		_introPan->setVisible(false);
	}
	else
	{
		_introPan->setVisible(true);		
	}
	if(_selectItemBox != itemBox)
	{
		if(_selectItemBox != nullptr)
			_selectItemBox->setFocusd(false);
		_selectItemBox = itemBox;
		_selectItemBox->setFocusd(true);
		updateIntro();
	}
}

void ItemBagPanel::onClose(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		destroy();
	}
}

void ItemBagPanel::onUse(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto typeId = _selectItemBox->getItem();
		if(typeId <= 0)return;
		/*EventHelper::getInstance()->dispatchByStr("target:"+Detect::shareDetect()->getPlayer()->getName()+"|type:useProp|senderType:Prop|senderTypeId:"+cocos2d::Value(typeId).asString(),"num:1|inBag:1");*/
		auto owner = dynamic_cast<Role*>(_itemBag->getOwner());
		if(owner == nullptr) return;
		// 日记本
		if(typeId == 2007002)
		{
			destroy();
			auto player = dynamic_cast<Player*>(owner);
			if(player != nullptr)
				PopPanel::getInstance()->addPanel(player->getDiary(),1);
		}
		else if(typeId == 2007001)
		{
			destroy();
			PopPanel::getInstance()->addPanel(SparDiskPanel::create("sparDisk"),1);
		}
		else if(typeId == 2004011)
		{
			destroy();
			auto equip = _itemBag->getEquipByType(10);
			auto owner = _itemBag->getOwner();
			owner->setModelByName("shopId", equip->getModelByName("shopId"));
			auto swap = Swap::create(owner->getName(), owner->getName());
			addChild(swap);
		}
		else
		{
			// 
			if(typeId == 2002010 || typeId == 2002011 || typeId == 2002012 || typeId == 2002016
				 || typeId == 2002017 || typeId == 2002018 || typeId == 2002019 || typeId == 2002020 || typeId == 2002021)
			{
				destroy();
			}
			owner->usePropByType(typeId,1);
		}
		updateItems();
		updateIntro();
	}
}

void ItemBagPanel::updateIntro()
{
	if(_selectItemBox == nullptr)
	{
		_introPan->setVisible(false);
		return;
	}
	auto typeId = _selectItemBox->getItem();
	if(typeId <= 0)
	{
		_introPan->setVisible(false);
		return;
	}
	_introPan->setVisible(true);
	ValueMap model;
	model["name"] = "itemBagTemp";
	model["typeId"] = typeId;
	auto cell = CellFactory::getInstance()->createCell(model);

	auto icell = dynamic_cast<Item*>(cell);
	if(icell == nullptr) return;

	Item* item = nullptr;
	if(cell->getType() == "Equip")
		item = _itemBag->getEquipByType(icell->getTypeId1());
	else if(cell->getType() == "Prop")
		item = _itemBag->getPropByType(icell->getTypeId());
	else if(cell->getType() == "Special")
		item = _itemBag->getSpecialByType(icell->getTypeId());

	if(item == nullptr) return;

	_nickNameLab->setString(item->getNickName());
	_introLab->setString(item->getModelByName("introduce").asString());
	_useBtn->setVisible(item->getModelByName("canUse").asBool());

	_durableLab->setVisible(true);
	if(item->getDurable() > 0)
	{
		_durableLab->setString(a2u("耐久:")+cocos2d::Value(item->getDurable()).asString());
	}
	else
	{
		_durableLab->setVisible(false);
	}
	
}