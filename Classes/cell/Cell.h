#ifndef __CELL_H__
#define __CELL_H__

#include "cocos2d.h"
#include "Global.h"
#include "model/Model.h"
#include "utils/EventHelper.h"

USING_NS_CC;
class Floor;
class Unit;

class Shader:public Ref
{
public:
	static Shader* create(std::string pshade, std::string pmask);
	Shader(std::string pshade, std::string pmask);
	~Shader();
	bool init();
	Sprite* shade;
	Sprite* mask;
	void update(Vec2 pos);
};

class Cell : public Layer, public TObject
{
public:
	~Cell();
	// Œª÷√
	/*CC_SYNTHESIZE(std::string, _floorName, FloorName);
	CC_SYNTHESIZE(std::string, _towerName, TowerName);
	CC_SYNTHESIZE(int, _floorId, FloorId);*/
	virtual Vec2 getSite() const;
	virtual void setSite(const Vec2 &vec);
	void updateSite();
	virtual int getLayer();
	virtual void setLayer(int id);
	virtual int isBlock();		
	virtual ValueMap saveModel();
	std::string getType();
	virtual int getTypeId();
	virtual std::string getImageUrl();
	virtual std::string getNickName();
	virtual std::string getEffect();
	virtual void setBlockFunc(std::function<int(Vec2&)> func);
	virtual Shader* getShader(){return _shader;}
	virtual void destroy();
	virtual void detachFromUnit();
	virtual void removeFromFloor();
	CC_SYNTHESIZE(Unit*, _unit, Unit);
	CC_SYNTHESIZE(Floor*, _floor, Floor);
	const cocos2d::Value getModelByName(std::string name);
	void setModelByName(std::string name, cocos2d::Value value);
	virtual void loadModel(ValueMap &map);
	virtual void refresh();
	virtual void refreshModel();
	CC_SYNTHESIZE(bool, _busy, Busy);
	CC_SYNTHESIZE(int, _mutual, Mutual);

	Sprite* getView(){return _view;}
	void disappear();
protected:
	bool init();
	virtual void initNull();
	virtual void initView();
	virtual void initModel();
	virtual void initOthers();
	virtual bool onTrigger(EventCustom *event);
	void onEnter();
	void onExit();
	virtual void moveToNewSite();
	virtual void stepToNewSite();
	virtual bool setAnimation(const std::string &str, bool loop = false, CallFuncN* callBack = nullptr, bool again = false, float duration = -1.0);
	const std::string getNameId();
protected:
	std::function<int(Vec2&)> _blockFunc;
	Vec2 _site;
	Vec2 _lastSite;
	int _layer;
	ValueMap _animations;
	Action *_curAnimate;
	Animation *_curAnimation;
	ValueMap _effect;
	ValueMap _triggerOn;

	Sprite* _view;
	bool _refresh;
	Shader* _shader;
	
#if DETECT_ACTOR == 1
public:
	void setLabel(std::string str1, std::string str2);
protected:
	Label *_label1;
	Label *_label2;
	
#endif
};

#endif /*__CELL_H__*/