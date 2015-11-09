#ifndef __ALTAR_H__
#define __ALTAR_H__

#include "Global.h"
#include "cocos2d.h"
#include "cell/item/Item.h"

class Altar : public Node, public TObject
{
public:
	Altar();
	~Altar();
	static Altar* create();
	void check();

	void initModel();
	void loadModel(ValueMap &map);
	ValueMap saveModel();
	bool onTrigger(EventCustom *event);
	const std::string getNameId();
	std::string getType();

	void reset();
private:
	ValueVector _removeIds;
	ValueVector forms;
	std::map<int, std::vector<std::vector<int>>> maps;
	std::vector<std::string> _switchs; 
	bool checkMap(std::vector<std::vector<int>> map);
};

#endif /*__ALTAR_H__*/