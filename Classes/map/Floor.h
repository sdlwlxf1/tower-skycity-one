#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "cocos2d.h"
#include "TObject.h"
#include "cell/Cell.h"
#include "utils/Shake.h"
#include "utils/Altar.h"

USING_NS_CC;

class Tower;

class Unit : public Ref
{
public:
	static Unit* create(Vec2 vec);
	Unit(Vec2 &site):_site(site){}
	CC_SYNTHESIZE_READONLY(Vec2,_site,Site);
	void attach(Cell *cell);
	void detach(Cell *cell);
	void center(Cell *cell);
	void touch(Cell *cell);
	int isBlock();
	void clearCells();
	Vector<Cell*> getCells();
private:
	Vector<Cell*> _cells;
};


class Floor:public Layer, public TObject
{
public:
	Floor(int id);
	Floor(ValueMap model);
	static Floor* create(int id);
	static Floor* create(ValueMap model);
	const int getId() const;

	virtual bool init();
	virtual void loadModel(ValueMap &map);
	virtual void initModel();
	virtual void initOthers();
	virtual ValueMap saveModel();
	~Floor();	
	virtual void onEnter();
	virtual void onExit();

	void setWeather(std::string type);
	void update(float delta);
	void stopWeather();

	void removeAllCells();
	void addCell(Cell* cell, Point site, int layerId = 1);
	bool removeCell(Cell* cell);
	//bool removeCellByName(std::string name);


	/*bool removeCellByType(int type);
	bool removeCellByType(int type, Vec2 site);*/


	//Cell* getCellByName(std::string name);

	Cell* getCellByType(int type);
	Cell* getCellByType(int type, Vec2 site);

	virtual bool onTrigger(EventCustom *event);
	int isBlockBySite(const Point &p);
	int isBlock(){return 0;}
	const std::string getNameId();
	std::string getType();

	Vector<Cell*> getAllActor();
	Vector<Cell*> getAllItem();

	Map<int, Cell*> getAllActorType();
	Map<int, Cell*> getAllItemType();

	void addShade(Shader* shader);
	void removeShade(Shader* shader);
	void addShadeAt(std::string fShade, std::string fmask, Vec2 pos);

	Unit* getUnit(Vec2 site);
	void attachToUnit(Cell* cell, Vec2 site);
	void detachFromUnit(Cell* cell, Vec2 site);
	void centerToUnit(Cell* cell, Vec2 site);
	void touchUnit(Cell* cell, Vec2 site);
	CC_SYNTHESIZE(Tower*, _tower,Tower);
	bool mapInited;
	void initMap();
	void buildMap();
	Altar* altar;
	void refreshMap();

	void lighting();

	Cell* getNearByTypeId(Vec2 center, const Vec2* vecs, int len1,const int* typeId, int len2);
	Cell* getNearByType(Vec2 center, const Vec2* vecs, int len1,const std::string* types, int len2);
private:
	void lightUpdate(float dt);
	void drawShanDian();
	void drawLighting(float x1, float y1, float x2, float y2, float displace);
	void initCells(ValueVector &objects);
	void updateMap(float dt);
private:
	bool _cellInited;
	bool _mapBuilded;
	Layer *_mapTemp;
	TMXTiledMap *_map;
	TMXTiledMap *_map0;
	int _id;
	TMXLayer *_ground;
	TMXLayer *_above;
	TMXLayer *_under;
	TMXObjectGroup *_stageGroup;
	Layer *_item;
	Layer *_stage;
	Vector<Layer*> _innerLayers;
	Map<int, Unit*> _units;
	Layer *_dark;
	LayerColor* _pLayer;

	//иа╣Г
	Action* _lightAction;
	LayerColor* _lightColor;
	int lightCount;
	int lightNum;
	Layer *_light;
	DrawNode *_lightDraw;


	Vector<Cell*> _cells;

#if DETECT_ACTOR == 1
public:
	void updateFight();
#endif	
};



#endif /*__FLOOR_H__*/

