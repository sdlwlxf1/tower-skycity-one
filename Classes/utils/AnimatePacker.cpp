#include "AnimatePacker.h"
#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class AnimateSaxDelegator : public SAXDelegator
{
public:
	enum{
		STATE_NONE,
		STATE_PLIST,
		STATE_ANIMATION,
		STATE_NAME,
		STATE_DELAY,
		STATE_FLIP_X,
		STATE_FLIP_Y,
		STATE_SPRITE_FRAME
	}state;

	void startElement(void *ctx, const char *name, const char **atts) ;
	void endElement(void *ctx, const char *name) ;
	void textHandler(void *ctx, const char *s, int len) ;

	vector<string> plists;//All plist name
	vector<AnimatePacker::Animate> animates;//All animate data
};

static AnimatePacker* animatePacker = nullptr;

AnimatePacker* AnimatePacker::getInstance()
{
	if(animatePacker == nullptr)
	{
		animatePacker = new AnimatePacker();
	}
	return animatePacker;
}

void AnimateSaxDelegator::startElement( void *ctx, const char *name, const char **atts )
{
	string tag((char*)name);

	if (tag=="plist")
	{
		state=STATE_PLIST;
	} 
	else if (tag=="animation")
	{
		state=STATE_ANIMATION;
		animates.push_back(AnimatePacker::Animate());
	}
	else if (tag=="name")
	{
		state=STATE_NAME;
	}
	else if (tag=="delay")
	{
		state=STATE_DELAY;
	}
	else if (tag=="spriteFrame")
	{
		state=STATE_SPRITE_FRAME;
	}
	else if (tag=="flipX")
	{
		state=STATE_FLIP_X;
	}
	else if (tag=="flipY")
	{
		state=STATE_FLIP_Y;
	}
	else
	{
		state=STATE_NONE;
	}
}

void AnimateSaxDelegator::endElement( void *ctx, const char *name )
{
	string tag((char*)name);

	if (tag=="plist")
	{
	} 
	else if (tag=="animation")
	{
	}
	else if (tag=="name")
	{
	}
	else if (tag=="delay")
	{
	}
	else if (tag=="spriteFrame")
	{
	}
	else if (tag=="flipX")
	{
	}
	else if (tag=="flipY")
	{
	}
	else
	{
	}

	state = STATE_NONE;
}

void AnimateSaxDelegator::textHandler( void *ctx, const char *ch, int len )
{
	if (state == STATE_NONE)
	{
		return;
	}

	string text((char*)ch,0,len);
	int index;
	float delay;

	switch (state)
	{
	case STATE_PLIST:
		plists.push_back(text);
		break;
	case STATE_ANIMATION:
		break;
	case STATE_NAME:
		index=animates.size()-1;
		animates[index].name=text;
		break;
	case STATE_DELAY:
		index=animates.size()-1;
		delay=atof(text.c_str());
		animates[index].delay=delay;
		break;
	case STATE_SPRITE_FRAME:
		index=animates.size()-1;
		animates[index].spriteFrames.push_back(text);
		break;
	case STATE_FLIP_X:
		index=animates.size()-1;
		animates[index].flipX=(text=="true");
		break;
	case STATE_FLIP_Y:
		index=animates.size()-1;
		animates[index].flipY=(text=="true");
		break;
	default:
		break;
	}

}

void AnimatePacker::loadAnimations(const char *path )
{
	std::string pszPath = FileUtils::getInstance()->fullPathForFilename(path);

	SAXParser parser;
	AnimateSaxDelegator delegator;

	if (false == parser.init("UTF-8"))
	{
		//TODO
		return;
	}
	parser.setDelegator(&delegator);
	parser.parse(pszPath);

	//load plist
	vector<string> plists = delegator.plists;
	for (unsigned int i=0;i<plists.size();i++)
	{
	    string plistPath = FileUtils::getInstance()->fullPathFromRelativeFile(plists[i].c_str(), pszPath);
		SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistPath.c_str());
	}

	//load animate
	vector<Animate> animates = delegator.animates;
	Vector<SpriteFrame*> spriteFramesArray;

	set<string> animateNames;
	for (unsigned int i=0;i<animates.size();i++)
	{
		Animate animate=animates[i];
		vector<string> spriteFrames=animate.spriteFrames;

		for (unsigned int j=0;j<spriteFrames.size();j++)
		{
			animateNames.insert(spriteFrames[j]);
			SpriteFrame *spriteFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(spriteFrames[j].c_str());
			spriteFramesArray.pushBack(spriteFrame);
		}

		Animation *animation = Animation::createWithSpriteFrames(spriteFramesArray,animate.delay);
		AnimationCache::sharedAnimationCache()->addAnimation(animation,animate.name.c_str());

		spriteFramesArray.clear();
	}

	//for (unsigned int i=0;i<animates.size();i++)  
	//{  
	//	Animate animate=animates[i];
	//	vector<string> spriteFrames=animate.spriteFrames;
	//	for (unsigned int j=0;j<spriteFrames.size();j++)  
	//	{  
	//		//animateNames.insert(spriteFrames[j]);  
	//		CCLOG("spriteFrames[j] = %s", spriteFrames[j].c_str());  
	//		SpriteFrame *spriteFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(spriteFrames[j].c_str());  
	//		//            CCLOG("spriteFrame = %p", spriteFrame);  
	//		spriteFramesArray->addObject(spriteFrame);  
	//	}
	//	animateNames.insert(animate.name);
	//	Animation *animation = Animation::createWithSpriteFrames(spriteFramesArray,animate.delay);
	//	AnimationCache::sharedAnimationCache()->addAnimation(animation,animate.name.c_str());
	//} 
	
	//record animate
	for(unsigned int i=0;i<animates.size();i++){
		Animate animate=animates[i];
		nameToAnimateMap[animate.name]=animate;
	}

	//record plist
	pathToPlistsMap[path]=plists;

	//record Animate name
	pathToNameMap[path]=animateNames;
}

Animate* AnimatePacker::getAnimate(const char *name )
{
	Animation* animation = AnimationCache::sharedAnimationCache()->animationByName(name);

	if(animation)
	{
		return cocos2d::Animate::create(animation);
	}
	return NULL;
}

void AnimatePacker::freeAnimations(const char *path){
	//const char *pszPath = FileUtils::getInstance()->fullPathFromRelativePath(path);
	auto fullPath = FileUtils::getInstance()->fullPathForFilename(path);
	std::string pszPath;
	if (fullPath.find_last_of("/") != std::string::npos)
	{
		pszPath = fullPath.substr(0, fullPath.find_last_of("/") + 1);
	}

	vector<string> plists=pathToPlistsMap[path];
	for (unsigned int i=0;i<plists.size();i++)
	{
	    string plistPath = FileUtils::getInstance()->fullPathFromRelativeFile(plists[i].c_str(), pszPath);
		SpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(plistPath.c_str());
	}
	pathToPlistsMap.erase(path);
	
	set<string> animateNames=pathToNameMap[path];
	for (set<string>::iterator strItr=animateNames.begin();strItr!=animateNames.end();++strItr)
	{
		AnimationCache::sharedAnimationCache()->removeAnimationByName((*strItr).c_str());
		nameToAnimateMap.erase((*strItr));
	}
	pathToNameMap.erase(path);

}

Sequence* AnimatePacker::getSequence(const char *name){
	Animation* animation=AnimationCache::sharedAnimationCache()->animationByName(name);

	if(animation)
	{
		Animate animate = nameToAnimateMap[name];

		Array *actions = Array::create();
		actions->addObject(FlipX::create(animate.flipX));
		actions->addObject(FlipY::create(animate.flipY));
		actions->addObject(cocos2d::Animate::create(animation));

		Sequence *sequence= createSequence(actions);
		actions->removeAllObjects();

		return sequence;
	}
	return NULL;
}

Sequence *AnimatePacker::createSequence(Array *actions)
{
	CC_ASSERT(actions->count()>1);
	Sequence *seq = Sequence::createWithTwoActions((FiniteTimeAction*)actions->objectAtIndex(0),
		(FiniteTimeAction*)actions->objectAtIndex(1));
	for (unsigned int i = 2; i < actions->count(); ++i) {
		seq = Sequence::createWithTwoActions(seq, (FiniteTimeAction*)actions->objectAtIndex(i));
	}
	return seq;
}

Sequence *AnimatePacker::createSequence(FiniteTimeAction *pAction1, FiniteTimeAction *pAction2, ...)
{
	va_list params;
	va_start(params, pAction2);

	Sequence *pPrev = Sequence::createWithTwoActions(pAction1, pAction2);
	FiniteTimeAction *pNow = NULL;  

	while( pPrev ) {
		pNow = va_arg(params, FiniteTimeAction*);
		if (pNow)
		{
			pPrev = Sequence::createWithTwoActions(pPrev, pNow);
		}
		else
		{
			break;
		}
	}  
	va_end(params);
	return pPrev;
}
