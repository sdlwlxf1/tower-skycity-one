#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "cell/Cell.h"
#include "cell/item/ItemBag.h"
#include "cell/item/SparDisk.h"
#include "ui/DiaryPanel.h"
#include "utils/AStar.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
USING_NS_CC;

class Actor : public Cell
{
public:
	enum Movement
	{
		STOP,LEFT,UP,RIGHT,DOWN
	};
	static std::string MovementStr[5];

	// ¿ñ±©¡¢ÆÆ¼×¡¢±©»÷¡¢ÎüÑª¡¢Á¬»÷¡¢Èâ¶Ü¡¢Èõ»¯¡¢¸ñµ²¡¢·´µ¯¡¢ÉÁ±Ü
	enum FightBuff
	{
		KB,PJ,BJ,XX,LJ,RD,RH,GD,FT,SB
	};
	static const char* FightBuffStr[10];
	static const char* FightBuffRateStr[10];
	static const char* FightBuffDrateStr[10];
	static std::map<const char*, int> FightBuffRateStrToInt;
	static std::map<const char*, int> FightBuffDrateStrToInt;
	static void buildMap();

	typedef Movement Direction;
	static Direction reverse(Direction dir){if(dir==UP){return DOWN;}
	else if(dir==DOWN){return UP;}
	else if(dir==LEFT){return RIGHT;}
	else if(dir==RIGHT){return LEFT;}
	return STOP;}

	enum AnimationType
	{
		IDLE,WALK,HIT
	};
	static std::string AnimationTypeStr[3];

	Actor(int typeId);
	Actor(ValueMap& model);
	~Actor();
	static Actor* create(int typeId);
	static Actor* create(ValueMap& model);
	int getId();
	virtual std::string getType(){return "Actor";}
	// override Sprite
	void onEnter();
	void update(float delta);

	// override TObject
	virtual void setSite(const Vec2 &vec);
	virtual void initNull();
	virtual void initModel();
	virtual void loadModel(ValueMap &map);
	virtual void initOthers();
	virtual ValueMap saveModel();
	virtual bool onTrigger(EventCustom *event);
	virtual int isBlock();

	virtual void addHp(int hp);
	virtual void addStr(int str);
	virtual void addDef(int def);
	virtual void addXp(int xp);
	virtual void addLevel(int level);
	virtual void addGold(int gold);
	virtual void addSpeed(float speed);

	virtual void addFightBuffRate(FightBuff id, float num);
	virtual void addFightBuffDrate(FightBuff id, float num);

	int getHp();
	int getStr();
	int getDef();
	int getXp();
	float getSpeed();
	const int getLevel(){return _level;}
	const int getGold(){return _gold;}

	double getDurTime();
	void setDurTime(double time){_durTime = time;}

	float getFightBuffRate(FightBuff id);
	float getFightBuffDrate(FightBuff id);
	

	CC_SYNTHESIZE(bool, _isDead, IsDead);
	CC_SYNTHESIZE(int, _chatId, ChatId);
	void setConveyCount(int n){_conveyCount = n;}
	virtual void setCanGuide(bool b, float delay = 0);
	bool getCanGuide(){return _canGuide;}
	void setCanWalk(bool b){_canWalk = b;}
	bool getCanWalk(bool b){return _canWalk;}
	void setSwapCount(int n){_swapCount = n;}
	int getSwapCount(){return _swapCount;}

	virtual void refreshModel();
protected:
	virtual void setHp(int hp);
	virtual void setStr(int str);
	virtual void setDef(int def);
	virtual void setXp(int xp);
	virtual void setLevel(int level);
	virtual void setGold(int gold);
	virtual void setSpeed(float speed);

	virtual void setFightBuffRate(FightBuff id, float value);
	virtual void setFightBuffDrate(FightBuff id, float value);

	void setNickName(std::string nickName);
	virtual void initView();
	virtual void animate(AnimationType type, Direction dir);
	virtual void moveToNewSite();
	virtual void stepToNewSite();
	virtual void dead();
	virtual void setPosition(const Vec2& pos) override;
protected:
	bool _canGuide;
	bool _canWalk;
	Sprite *_view;
	Movement _guide;
	ItemBag *_itemBag;
	SparDisk *_sparDisk;
	DiaryPanel *_diary;
	int _conveyCount;
	int _swapCount;


	std::string _rival;
	// ÊôÐÔ
	std::string _nickName;
	std::string _type;
	int _typeId;
	int _level;
	int _hp;
	int _str;
	int _def;
	int _xp;
	int _gold;
	int _fw;
	int _maxHp;
	int _nextXp;
	int _lastXp;
	float _speed;
	float _saveSpeed;

	long _initTime;
	double _durTime;

	// ×´Ì¬
	Movement _move;
	Direction _face;
	AnimationType _ani;
	float _fightBuffRate[10];
	float _fightBuffDrate[10];
	// Î»ÖÃ
	Vec2 _nextSite;
	Vec2 _centerSite;
	Vec2 _faceSite;

	
#if DETECT_ACTOR == 1
public:
	void setDamageLabel(std::string damage, std::string win);
	void setNumLabel(std::string num);
	void setNum2Label(std::string num);
	void setNum3Label(std::string num);
protected:
	Label *_damageLabel;
	Label *_numLabel;
	Label *_num2Label;
	Label *_num3Label;
#endif
};


class Monster:public Actor
{
public:
	static Monster* create(ValueMap& model);
	Monster(ValueMap& model):Actor(model){}
	std::string getType(){return "Monster";}
	virtual bool onTrigger(EventCustom *event);
};

class Role : public Actor
{
public:
	~Role();
	Role(int typeId):Actor(typeId){}
	Role(ValueMap& model):Actor(model){}
	static Role* create(int id);
	static Role* create(ValueMap& model);
	virtual ValueMap saveModel();
	virtual void initNull();
	virtual void initModel();
	virtual void loadModel(ValueMap &map);
	virtual void initOthers();
	virtual std::string getType(){return "Role";}
	void setBlockFunc(std::function<int(Vec2&)> func);
	//void setFace(Direction dir);
	virtual void setCanGuide(bool b, float delay = 0);
	virtual void getGemByType(int typeId, int num = 1);
	virtual void usePropByType(int typeId, int num = 1);

	void setFightDelay(float delay){_fightDelay = delay;}
	float getFightDelay();

	CC_SYNTHESIZE(int, _useHoeNum, UseHoeNum);
	CC_SYNTHESIZE(int, _useBombNum, UseBombNum);
protected:
	void update(float delta);
	void onEnter();
	void moveTo(Vec2 dest);
	void guideTo(Vec2 vec);
	void setGuide(Direction dir,int value);
	virtual void convey(Vec2 site, int layerId, int floorId, int towerId);
	virtual void addEquip(Equip *equip);
	virtual void addSpar(Spar *spar);
	virtual void addProp(Prop *prop);
	virtual void addSpecial(Special *special);
	virtual void removeProp(Prop *prop);

	virtual void addEquipByType(int typeId);
	virtual void addPropByType(int typeId, int num = 1);
	virtual void removePropByType(int typeId, int num = 1);

	virtual void addSpecialByType(int typeId, int num = 1);
	virtual void removeSpecialByType(int typeId, int num = 1);

	
	virtual void addSparByType(int typeId);
	
	virtual bool onTrigger(EventCustom *event);

	virtual void moveToNewSite();
	virtual void stepToNewSite();

	virtual void dead();
protected:
	Vec2 _dis;
	PointArray *_paths;
	int _guides[5];
	PointArray *_vision;
	float _fightDelay;
private:
	Point getNearSite(Point core);
	void updateGuide();
	void walk();
	bool nextTileCanMove(Movement);
	void switchMovement(Movement);
	bool checkCenterTile();
	Point changeGuide();
private:
	AStar *_astar;
};


class Player : public Role
{
public:
	Player(int typeId):Role(typeId){}
	Player(ValueMap& model):Role(model){}
	static Player* create(int id);
	static Player* create(ValueMap& model);
	std::string getType(){return "Player";}
	void onEnter();
	void update(float delta);
	virtual void initNull();
	virtual void initModel();
	virtual void loadModel(ValueMap &map);
	virtual ValueMap saveModel();
	virtual void initOthers();
	virtual bool onTrigger(EventCustom *event);
	ItemBag* getItemBag();
	SparDisk* getSparDisk();
	DiaryPanel* getDiary();
	virtual void addEquipByType(int typeId);
	virtual void addPropByType(int typeId, int num = 1);
	virtual void addSpecialByType(int typeId, int num);
	virtual void addSpecial(Special *special);
	virtual void addEquip(Equip *equip);
	virtual void addSpar(Spar *spar);
	virtual void addProp(Prop *prop);
	virtual void removeProp(Prop *prop);
	virtual void removePropByType(int typeId, int num = 1);

	virtual void addSparByType(int typeId);

	virtual void refresh();
	
	virtual void updateToHud();
	//~Player();

	virtual void addHp(int hp);
	virtual void addStr(int str);
	virtual void addDef(int def);
	virtual void addXp(int xp);
	virtual void addGold(int gold);
	virtual void addLevel(int level);

	virtual void setHp(int hp);
	virtual void setStr(int str);
	virtual void setDef(int def);
	virtual void setXp(int xp);
	virtual void setGold(int gold);
	virtual void setLevel(int level);

	
protected:
	virtual void convey(Vec2 site, int layerId, int floorId, int towerId);
	virtual void dead();
private:
	void moveToNewSite();

	
	
};


#endif /*__ACTOR_H__*/