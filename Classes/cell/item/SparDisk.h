#ifndef __SPAR_DISK_H__
#define __SPAR_DISK_H__

#include "cocos2d.h"
#include "TObject.h"
#include "Item.h"

class Actor;

USING_NS_CC;

class SparDisk : public Ref, public TObject
{
public:

	void equipSpar(int index, int typeId);
	void getOffSpar(int index);

	void bagSpar(int typeId);
	void takeOutSpar(int typeId);

	void addSpar(Spar* spar);
	Spar* getSpar(int typeId);

	void addSparByType(int typeId);

	SparDisk();
	SparDisk(ValueMap &model);
	//~Item();
	static SparDisk* create();
	static SparDisk* create(ValueMap &model);
	bool init();

	CC_SYNTHESIZE(float, _addHp, AddHp);
	CC_SYNTHESIZE(float, _addStr, AddStr);
	CC_SYNTHESIZE(float, _addDef, AddDef);

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
	std::string getType(){return "SparDisk";}
	bool onTrigger(EventCustom *event);

	Map<int, Spar*>& getEquipSpars(){return _equipSpars;}
	Map<int, Spar*>& getBagSpars(){return _bagSpars;}
protected:
	Map<int, Spar*> _spars;

	Map<int, Spar*> _bagSpars;
	Map<int, Spar*> _equipSpars;

	Actor* _owner;

	float _addFightBuffRate[10];
	float _addFightBuffDrate[10];
};

#endif /*__SPAR_DISK_H__*/