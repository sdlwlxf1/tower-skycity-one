#include "AStar.h"

USING_NS_CC;

Vec2 AStar::CrossVecs[4] = {Vec2(-1,0),Vec2(0,1),Vec2(1, 0),Vec2(0,-1)};

AStar* AStar::create()
{
	AStar *pRet = new AStar();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}	
}

AStar::~AStar()
{
	release();
}

bool AStar::init()
{
	return true;
}

AStar::AStar():_function(nullptr){}

void AStar::initCheckPointFunc(const std::function<bool(Point&)> &func)
{
	_function = func;
}

PointArray* AStar::getShortPath(Point start, Point end)
{
	_shortPath = PointArray::create(20);

	/*if (start == end)
	{
	LOG("You're already there! :P");
	return;
	}*/
	/*if (!_layer->isValidTileCoord(end) || _layer->isWallAtTileCoord(end)){
	SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
	return;
	}*/
	//LOG("Start: %f, %f", start.x, start.y);
	//LOG("End: %f, %f", end.x, end.y);
	//bool pathFound = false;
	_openCells.clear();
	_closeCells.clear();
	// 首先，添加起始坐标到open列表
	this->insertInOpenCells(Cell::createWithPosition(start));
	do{
		// 得到最小的F值步骤
		// 因为是有序列表，第一个步骤总是最小的F值
		Cell *curCell = _openCells.at(0);
		// 添加当前步骤到closed列表
		_closeCells.pushBack(curCell);
		// 将它从open列表里面移除
		// 需要注意的是，如果想要先从open列表里面移除，应小心对象的内存
		_openCells.erase(0);
		// 如果当前步骤是目标方块坐标，那么就完成了
		if (curCell->getPosition() == end)
		{
			buildPath(curCell);
			_openCells.clear();
			_closeCells.clear();
			break;
		}
		// 得到当前步骤的相邻方块坐标
		getBordorPoints(curCell->getPosition());
		for (ssize_t i = 0; i < _points->count(); ++i)
		{
			Cell *cell = Cell::createWithPosition(_points->getControlPointAtIndex(i));
			// 检查步骤是不是已经在closed列表			
			if(getCellIndex(_closeCells, cell) != -1)
			{
				continue;
			}
			// 计算从当前步骤到此步骤的成本
			int cost = this->cellCost(curCell, cell);
			// 检查此步骤是否已经在open列表
			ssize_t index = getCellIndex(_openCells, cell);
			// 不在open列表，添加它
			if (index == -1)
			{
				// 设置当前步骤作为上一步操作
				cell->setParent(curCell);
				// G值等同于上一步的G值 + 从上一步到这里的成本
				cell->setGScore(curCell->getGScore() + cost);
				// H值即是从此步骤到目标方块坐标的移动量估算值
				cell->setHScore(this->computeHScore(cell->getPosition(), end));
				// 按序添加到open列表
				this->insertInOpenCells(cell);
			}
			else
			{
				// 获取旧的步骤，其值已经计算过
				cell = _openCells.at(index);
				// 检查G值是否低于当前步骤到此步骤的值
				if ((curCell->getGScore() + cost) < cell->getGScore())
				{
					// 设置当前步骤作为上一步操作
					cell->setParent(curCell);
					// G值等同于上一步的G值 + 从上一步到这里的成本
					cell->setGScore(curCell->getGScore() + cost);
					// 因为G值改变了，F值也会跟着改变
					// 所以为了保持open列表有序，需要将此步骤移除，再重新按序插入
					// 在移除之前，需要先保持引用
					cell->retain();
					// 现在可以放心移除，不用担心被释放
					_openCells.erase(index);
					// 重新按序插入
					this->insertInOpenCells(cell);
					// 现在可以释放它了，因为open列表应该持有它
					cell->release();
				}
			}
		}
	} while (_openCells.size() > 0);

	_shortPath->insertControlPoint(start,0);


	/*for(int l = 0; l < _shortPath->count(); l++)
	{
	auto v = _shortPath->getControlPointAtIndex(l);
	log("%f       %f", v.x, v.y);
	}*/

	return _shortPath;
}

ssize_t AStar::getCellIndex(cocos2d::Vector<Cell*> &cells, Cell *cell)
{	
	for (ssize_t i = 0; i < cells.size(); ++i){
		if (cells.at(i)->isEqual(cell)){
			return i;
		}
	}
	return -1;
}

//直角方向行走
void AStar::getBordorPoints(Point core)
{
	_points = PointArray::create(4);	
	for(auto unit : CrossVecs)
	{
		Point p(core);
		p += unit;
		if(_function)
		{
			if(_function(p) == 0)
			{
				_points->addControlPoint(p);
			}			
		}else
		{
			_points->addControlPoint(p);
		}
	}
}

void AStar::buildPath(const Cell *cell)
{
	do{
		// 起始位置不要进行添加
		if (cell->getParent())
		{
			// 总是插入到索引0的位置，以便反转路径
			Vec2 p = cell->getPosition();
			_shortPath->insertControlPoint(p, 0);
		}
		cell = cell->getParent();   // 倒退
	} while (cell);                 // 直到没有上一步
}


void AStar::insertInOpenCells(Cell *cell)
{
	int cellFScore = cell->getFScore();
	ssize_t count = _openCells.size();
	ssize_t i = 0;
	for (; i < count; ++i)
	{
		if (cellFScore <= _openCells.at(i)->getFScore())
		{
			break;
		}
	}
	_openCells.insert(i, cell);
}

int AStar::computeHScore(Point start, Point end)
{
	// 这里使用曼哈顿方法，计算从当前步骤到达目标步骤，在水平和垂直方向总的步数
	// 忽略了可能在路上的各种障碍
	return abs(end.x - start.x) + abs(end.y - start.y);
}

int AStar::cellCost(const Cell *start, const Cell *end){
	//return ((fromCell->getPosition().x != toCell->getPosition().x)
	//	&& (fromCell->getPosition().y != toCell->getPosition().y)) ? 14 : 10;
	return 1;
}


AStar::Cell::Cell() :
	_position(Point::ZERO),
	_gScore(0),
	_hScore(0),
	_parent(nullptr)
{
}

AStar::Cell::~Cell()
{
}

AStar::Cell *AStar::Cell::createWithPosition(Point pos)
{
	AStar::Cell *pRet = new AStar::Cell();
	if (pRet && pRet->initWithPosition(pos))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool AStar::Cell::initWithPosition(Point pos)
{
	bool bRet = false;
	do
	{
		this->setPosition(pos);
		bRet = true;
	} while (0);

	return bRet;
}



bool AStar::Cell::isEqual(const AStar::Cell *other) const
{
	return this->getPosition() == other->getPosition();
}

std::string AStar::Cell::getDescription() const
{
	return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d",
		this->getPosition().x, this->getPosition().y,
		this->getGScore(), this->getHScore(), this->getFScore());
}