#ifndef __ITEMBAG_H__
#define __ITEMBAG_H__

#include "cocos2d.h"
#include "TObject.h"
#include "Item.h"

class Actor;

USING_NS_CC;

class ItemBag : public Ref, public TObject
{
public:
	void addEquip(Equip* equip);
	void addEquipByType(int typeId);
	void removeEquip(Equip* equip);
	void removeEquipByType(int typeId);
	Equip* getEquipByType(int typeId);

	void addProp(Prop* prop);
	void addPropByType(int typeId, int num = 1);
	void removeProp(Prop* prop);
	void removePropByType(int typeId, int num = 1);
	Prop* getPropByType(int typeId);

	void addSpecial(Special* special);
	void addSpecialByType(int typeId, int num = 1);
	void removeSpecial(Special* special);
	void removeSpecialByType(int typeId, int num = 1);
	Special* getSpecialByType(int typeId);

	int getPropSizeByType(int typeId);
	int getSpecialSizeByType(int typeId);

	int getCapacity();
	int getCount();

	void tidy();

	ItemBag();
	ItemBag(ValueMap &model);
	//~Item();
	static ItemBag* create();
	static ItemBag* create(ValueMap &model);
	bool init();

	CC_SYNTHESIZE(int, _addHp, AddHp);
	CC_SYNTHESIZE(int, _addStr, AddStr);
	CC_SYNTHESIZE(int, _addDef, AddDef);

	void setAddFightBuffRate(int fb, float value){ _addFightBuffRate[fb] = value;}
	void setAddFightBuffDrate(int fb, float value){ _addFightBuffDrate[fb] = value;}

	float getAddFightBuffRate(int fb){return _addFightBuffRate[fb];}
	float getAddFightBuffDrate(int fb){return _addFightBuffDrate[fb];}

	void updateAddData();
	void setOwner(Actor* owner);
	Actor* getOwner();
// override
	void initModel();
	void loadModel(ValueMap &map);
	ValueMap saveModel();
	const std::string getNameId();
	std::string getType(){return "ItemBag";}
	bool onTrigger(EventCustom *event);
	int getGold();
	void setGold(int gold);
	void addGold(int gold);
	Map<int, Equip*>& getEquips(){return _equips;}
	std::map<int, Vector<Prop*>>& getProps(){return _props;}
	std::map<int, Vector<Special*>>& getSpecials(){return _specials;}
protected:
	Map<int, Equip*> _equips;
	std::map<int, Vector<Prop*>> _props;
	std::map<int, Vector<Special*>> _specials;
	int _gold;
	Actor* _owner;

	float _addFightBuffRate[10];
	float _addFightBuffDrate[10];
};

#endif /*__ITEMBAG_H__*/