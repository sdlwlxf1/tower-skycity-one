#ifndef __TOWER_H__
#define __TOWER_H__

#include "cocos2d.h"
#include "TObject.h"
#include "map/Floor.h"
#include "utils/Shake.h"

class World;

USING_NS_CC;

class Tower:public Layer, public TObject
{
public:
	Tower(int id);
	Tower(ValueMap model);
	static Tower* create(int id);
	static Tower* create(ValueMap model);
	const int getId() const;
	std::string getNickName();

	virtual bool init();
	virtual void initModel();
	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();
	~Tower();
	virtual void onEnter();
	virtual void onExit();
	//void add(TObject* object, int floorId, Vec2 site);
	int isBlockBySite(const Point &p);
	const std::string getNameId();
	virtual std::string getType(){return "Tower";}
	std::map<int, bool> getUnlockFloorId();
	CC_SYNTHESIZE(int, _floorsNum, FloorsNum);
	Floor* getFloor(int floorId);
	Action* shake;
	CC_SYNTHESIZE(World*, _world,World);
	void removeAllCells();
	void refreshMap();
	CC_SYNTHESIZE(bool, _canConvey,CanConvey);
private:
	int isBlock(){return 0;}
	bool onTrigger(EventCustom *event);
	EventListenerCustom* _listener1;
private:
	ValueMap _floorUrls;
	// Â¥²ã»º´æ
	Map<int, Floor*> _floors;
	int _id;
};




#endif /*__TOWER_H__*/