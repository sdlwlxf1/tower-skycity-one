#ifndef __WORLD_H__
#define __WORLD_H__

#include "cocos2d.h"
#include "TObject.h"
#include "Tower.h"
#include "utils/Shake.h"

USING_NS_CC;

class World:public Layer, public TObject
{
public:
	World();
	World(ValueMap model);
	static World* create();
	static World* create(ValueMap model);
	virtual bool init();
	virtual void initModel();
	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();
	~World();
	virtual void onEnter();
	virtual void onExit();
	//void add(TObject* object, int towerId, Vec2 site);
	int isBlockBySite(const Point &p);
	const std::string getNameId();
	virtual std::string getType(){return "World";}
	std::map<int, bool> getUnlockTowerId();
	CC_SYNTHESIZE(int, _towersNum, TowersNum);
	Tower* getTower(int towerId);
	void removeAllCells();
	Action* shake;
	void removeTreasure(std::string name);
	std::map<std::string, std::vector<std::string>> getTreasures();
	std::string getRandTreasure(std::string type);
	void refreshMap();
private:
	int isBlock(){return 0;}
	bool onTrigger(EventCustom *event);
	EventListenerCustom* _listener1;
private:
	ValueMap _towerUrls;
	// Ëþ»º´æ
	Map<int, Tower*> _towers;

	std::map<std::string, std::vector<std::string>> _treasures;
};

#endif /*__WORLD_H__*/