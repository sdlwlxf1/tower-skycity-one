#ifndef _ANIMATE_PACKER_H_
#define _ANIMATE_PACKER_H_

#include <string.h>
//#include <map>
//#include <vector>
//#include <set>
#include "cocos2d.h"



class AnimatePacker
{
public:
	struct Animate{
		std::string name;
		float delay;
		bool flipX;
		bool flipY;
		std::vector<std::string> spriteFrames;
	};
	static AnimatePacker* getInstance();
	void loadAnimations(const char *path);
	void freeAnimations(const char *path);
	//Using this function to getting original animate(without FilpX and FlipY).
	cocos2d::Animate* getAnimate(const char *name);
	//This function supports FlipX and FlipY.
	cocos2d::Sequence* getSequence(const char *name);
private:
	//The two functions is came from Timothy Zhang. Thank him for his share.
	//Original Tip Link:http://www.cocos2d-x.org/boards/6/topics/7219
	cocos2d::Sequence *createSequence(cocos2d::Array *actions);
	cocos2d::Sequence *createSequence(cocos2d::FiniteTimeAction *pAction1, cocos2d::FiniteTimeAction *pAction2, ...);
	
	//From animate name to Animates
	std::map<std::string,Animate> nameToAnimateMap;
	//From xml path to plist names
	std::map<std::string,std::vector<std::string> > pathToPlistsMap;
	//From xml path to animate names
	std::map<std::string,std::set<std::string> > pathToNameMap;
};

#endif//_ANIMATE_PACKER_H_

