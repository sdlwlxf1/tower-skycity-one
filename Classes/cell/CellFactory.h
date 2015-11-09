#ifndef __TOBJECT_FACTORY_H__
#define __TOBJECT_FACTORY_H__

#include "utils/Singleton.h"
#include "Cell.h"

class CellFactory:public Singleton<CellFactory>
{
public:
	Cell* createCell(ValueMap& model);
};

#endif /*__TOBJECT_FACTORY_H__*/