#ifndef __BUFF_UNIT_H__
#define __BUFF_UNIT_H__

#include "UI/SimplePanel.h"
#include "buff/Buff.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

class BuffUnit : public Layout
{	
public:
	static BuffUnit* create();
	bool init();
	void setBuff(Buff* buff);
	Buff* getBuff();
	void onExit();
	void updateBuffTime(int curTime, int tolTime);
private:
	Buff* _buff;
	ProgressTimer* _endTimer;
};


#endif /*__BUFF_UNIT_H__*/