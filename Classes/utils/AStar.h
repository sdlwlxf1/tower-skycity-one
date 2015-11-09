#ifndef __ASTAR_H__
#define __ASTAR_H__

#include "cocos2d.h"

USING_NS_CC;


class AStar : public Ref
{
public:
	class Cell : public Ref
	{
	public:
		Cell();
		~Cell();
		static Cell *createWithPosition(Vec2 pos);
		bool initWithPosition(Vec2 pos);
		int getFScore() const{return this->getGScore() + this->getHScore();}
		bool isEqual(const Cell *other) const;
		std::string getDescription() const;
		CC_SYNTHESIZE(Vec2, _position, Position);
		CC_SYNTHESIZE(int, _gScore, GScore);
		CC_SYNTHESIZE(int, _hScore, HScore);
		CC_SYNTHESIZE(Cell*, _parent, Parent);	
	};
	~AStar();
	AStar();
	static AStar* create();
	bool init();
	static Vec2 CrossVecs[];//{Vec2(-1,0),Vec2(0,1),Vec2(1, 0),Vec2(0,-1)}
	PointArray* getShortPath(Point start, Point end);
	void initCheckPointFunc(const std::function<bool(Vec2&)> &func);
protected:
	void insertInOpenCells(Cell *cell);
	int computeHScore(Vec2 start,Vec2 end);
	int cellCost(const Cell *start, const Cell *end);
	ssize_t getCellIndex(Vector<Cell *> &Cells, Cell *cell);
	void getBordorPoints(Vec2 core);
	void buildPath(const Cell *cell);
private:
	static AStar* _instance;
	Vector<Cell*> _openCells;
	Vector<Cell*> _closeCells;
	PointArray *_shortPath;
	PointArray *_points;
	std::function<bool(Vec2&)> _function;
};


#endif /*__ACTOR_H__*/
