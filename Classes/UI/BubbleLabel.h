#ifndef __BUBBLE_LABEL_H__
#define __BUBBLE_LABEL_H__

#include "cocos2d.h"
USING_NS_CC;

class BubbleLabel:public Layer
{
public:
	static BubbleLabel* create();
	BubbleLabel();
	bool init();
	void addLabel(std::string info, Vec2 site, Color3B color = Color3B::WHITE);
protected:
	void check();
	void bubble();
	void bubbleOutLabel();
private:
	Vector<Label*> _vec;
	bool _bubbling;
};

#endif /*__BUBBLE_LABEL_H__*/