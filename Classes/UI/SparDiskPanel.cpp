#include "SparDiskPanel.h"
#include "utils/StringHandler.h"
#include "model/Model.h"
#include "PopPanel.h"
#include "Detect.h"

#define NONE_IMAGE "none.png"

SparBox* SparBox::create(Button* button,ImageView* bg,ImageView* image,TextAtlas* num)
{
	SparBox *pRet = new SparBox();
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

bool SparBox::init(Button* button,ImageView* bg,ImageView* image,TextAtlas* num)
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
	return true;
}

void SparBox::setFixSpar(int typeId)
{
	if(typeId <= 0)
		return;
	/*rapidjson::Document &datas = Model::shareModel()->itemDatas;
	auto &data = Model::GetDataFromCol1(datas,typeId);
	auto bg_image = DICHELPER->getStringValue_json(data,"image:string");
	auto typeId1 = DICHELPER->getIntValue_json(data,"typeId1:int");*/

	auto item = Spar::create(typeId);

	_bg->setCascadeColorEnabled(true);
	_bg->setColor(Color3B::BLACK);
	_bg->setOpacity(100);
	item->setAnchorPoint(Vec2(0.16,0));
	_bg->addChild(item);
	//setBg(bg_image);
	_fixTypeId = item->getTypeId1();
}

void SparBox::setSpar(int typeId)
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
	auto item = Spar::create(typeId);
	item->setAnchorPoint(Vec2(0.16,0));
	_image->addChild(item);
}

void SparBox::setNum(int n)
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
			setSpar(0);
		}
	}
}

SparDiskPanel* SparDiskPanel::create(std::string name)
{
	SparDiskPanel *pRet = new SparDiskPanel(name);
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

void SparDiskPanel::onEnter()
{
	Layer::onEnter();
}

void SparDiskPanel::onExit()
{
	Layer::onExit();
}

bool SparDiskPanel::init()
{
	_sparDisk = Detect::shareDetect()->getPlayer()->getSparDisk();

	_equipSparPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_equipSpar"));
	_bagSparPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_bagSpar"));
	_introPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_intro"));
	_nickNameLab = static_cast<Text*>(_introPan->getChildByName("Label_name"));
	_introLab = static_cast<Text*>(_introPan->getChildByName("Label_intro"));

	_refineBtn = static_cast<Button*>(_introPan->getChildByName("Button_refine"));

	_patchNumLab = static_cast<Text*>(_introPan->getChildByName("Label_patchNum"));
	_completeLab = static_cast<Text*>(_introPan->getChildByName("Label_complete"));

	_refineBtn->addTouchEventListener(CC_CALLBACK_2(SparDiskPanel::onRefine, this));

	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(SparDiskPanel::onButtonClicked, this));
	}

	int i = 0;
	// 初始化阴影
	// disks
	i = 0;
	for(auto sparPan:_equipSparPan->getChildren())
	{
		auto image = static_cast<ImageView*>(sparPan->getChildByName("Image_item"));
		auto bg = static_cast<ImageView*>(sparPan->getChildByName("Image_bg_item"));
		auto button = static_cast<Button*>(sparPan->getChildByName("Button_item"));
		auto num = static_cast<TextAtlas*>(sparPan->getChildByName("AtlasLabel"));
		auto sparBox = SparBox::create(button,bg,image,num);
		_equipSparBoxs.pushBack(sparBox);
		button->addTouchEventListener(CC_CALLBACK_2(SparDiskPanel::onEquipSparClicked, this));
		sparBox->setTag(i);
		i++;
	}

	// spar
	i = 0;
	for(auto sparPan:_bagSparPan->getChildren())
	{
		auto image = static_cast<ImageView*>(sparPan->getChildByName("Image_item"));
		auto bg = static_cast<ImageView*>(sparPan->getChildByName("Image_bg_item"));
		auto button = static_cast<Button*>(sparPan->getChildByName("Button_item"));
		auto num = static_cast<TextAtlas*>(sparPan->getChildByName("AtlasLabel"));
		auto sparBox = SparBox::create(button,bg,image,num);
		_sparBoxs.pushBack(sparBox);
		button->addTouchEventListener(CC_CALLBACK_2(SparDiskPanel::onSparClicked, this));
		sparBox->setTag(i);
		i++;

		auto name = sparPan->getName();
		ValueVector valueName;
		stringToValueVector(name, "_", valueName);
		auto typeId = valueName.at(valueName.size() - 1).asInt();
		sparBox->setFixSpar(typeId);
	}

	updateSpars();
	setSelectSparBox(nullptr);
	_selectEquipSparBox = _equipSparBoxs.at(0);
	return true;
}

void SparDiskPanel::onRefine(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto typeId = _introSpar;
		if(typeId <= 0)return;
		auto spar = _sparDisk->getSpar(typeId);
		auto owner = dynamic_cast<Player*>(_sparDisk->getOwner());
		if(owner == nullptr) return;
		auto sparPatchNum = owner->getItemBag()->getPropSizeByType(2002005);
		if(sparPatchNum >= spar->getCostPatchNum())
		{
			PopPanel::getInstance()->confirm("upConfirm","type:Text---text:"+spar->getUpConfirm(),PromptPanel::TYPE_DONE_CANCEL,[=](){
				owner->usePropByType(2002005,spar->getCostPatchNum());
				PopPanel::getInstance()->note("cantUpConfirm","type:Text---text:"+a2u("恭喜你，晶石提炼成功"), 1.0f, false, true);
				spar->upLevel();
				_sparDisk->updateAddData();
				updateIntro();
			},nullptr,true,true);
		}
		else
		{
			PopPanel::getInstance()->note("cantUpConfirm","type:Text---text:"+a2u("碎片数量不足")+cocos2d::Value(spar->getCostPatchNum()).asString()+a2u("个"), 1.0f, false, true);
		}
	}
}

void SparDiskPanel::clearSpars()
{
	for(auto equipSparBox:_equipSparBoxs)
	{
		equipSparBox->setNum(0);
	}
	for(auto sparBox:_sparBoxs)
	{
		sparBox->setNum(0);
	}
}

void SparDiskPanel::updateSpars()
{
	clearSpars();
	Map<int, Spar*>& bagSpars = _sparDisk->getBagSpars();
	Map<int, Spar*>& equipSpars = _sparDisk->getEquipSpars();

	for(auto pair:equipSpars)
	{
		auto sparBox = _equipSparBoxs.at(pair.first);
		sparBox->setSpar(pair.second->getTypeId());
	}

	for(auto spar:bagSpars)
	{
		for(auto sparBox:_sparBoxs)
		{
			if(spar.second->getTypeId1() == sparBox->getFixSpar())
			{
				sparBox->setSpar(spar.second->getTypeId());
				break;
			}
		}
	}
}

void SparDiskPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			{
				destroy();
			}			
			break;
		}
	}
}

void SparDiskPanel::onEquipSparClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) {
		Button *button = (Button*)ref;
		auto sparBox = static_cast<SparBox*>(button->getUserObject());
		setSelectSparBox(sparBox);
		_selectEquipSparBox = sparBox;
	}
}

void SparDiskPanel::onSparClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) {
		Button *button = (Button*)ref;
		auto sparBox = static_cast<SparBox*>(button->getUserObject());
		setSelectSparBox(sparBox);

		if(_selectEquipSparBox->getSpar() != 0)
			_sparDisk->getOffSpar(_selectEquipSparBox->getTag());
		if(sparBox->getSpar() != 0)
			_sparDisk->equipSpar(_selectEquipSparBox->getTag(),sparBox->getSpar());
		updateSpars();
	}
}

void SparDiskPanel::setSelectSparBox(SparBox* sparBox)
{
	if(sparBox == nullptr)
	{
		_selectSparBox = sparBox;
		_introPan->setVisible(false);
		return;
	}
	if(sparBox->getSpar() == 0)
	{
		_introPan->setVisible(false);
	}
	else
	{
		_introPan->setVisible(true);		
	}
	if(_selectSparBox != sparBox)
	{
		if(_selectSparBox != nullptr)
			_selectSparBox->setFocusd(false);
		_selectSparBox = sparBox;
		_selectSparBox->setFocusd(true);
		updateIntro();
	}
}

void SparDiskPanel::updateIntro()
{
	auto typeId = _selectSparBox->getSpar();
	_introSpar = typeId;
	if(typeId <= 0)
	{
		setSelectSparBox(nullptr);
		return;
	}
	
	auto spar = _sparDisk->getSpar(typeId);
	if(spar == nullptr) return;
	auto owner = dynamic_cast<Player*>(_sparDisk->getOwner());
	if(owner == nullptr) return;
	auto sparPatchNum = owner->getItemBag()->getPropSizeByType(2002005);
	_patchNumLab->setString(a2u("碎片:")+cocos2d::Value(sparPatchNum).asString()+a2u("  升级消耗:")+cocos2d::Value(spar->getCostPatchNum()).asString());

	std::string nickName;
	std::string introduce;
	nickName = spar->getNickName();
	introduce = spar->getModelByName("introduce").asString();

	stringReplace(introduce, "%rate", cocos2d::Value(int(spar->getRate() * 100)).asString()+"%");
	stringReplace(introduce, "%drate", cocos2d::Value(int(spar->getDrate() * 100)).asString()+"%");

	_nickNameLab->setString(nickName);
	_introLab->setString(introduce);

	if(spar->getRate() >= spar->getMaxRate() || spar->getDrate() >= spar->getMaxDrate())
	{
		_completeLab->setVisible(true);
		_refineBtn->setVisible(false);
	}
	else
	{
		_completeLab->setVisible(false);
		_refineBtn->setVisible(true);
	}
}