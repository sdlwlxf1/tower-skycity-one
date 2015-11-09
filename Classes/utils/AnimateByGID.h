#ifndef __ANIMATEBYGID_H__
#define __ANIMATEBYGID_H__

#include "cocos2d.h"
USING_NS_CC;

class AnimateByGID : public ActionInterval
{
public:
	/** creates the action with an ValueVector and will restore the original frame when the ValueVector is over */
	static AnimateByGID* create(ValueVector gids, TMXLayer* layer, Vec2 site,float duration);

	/** sets the ValueVector object to be animated */
	void setGids( ValueVector gids);
	/** returns the ValueVector object that is being animated */
	ValueVector getGids() { return _gids; }
	const ValueVector getGids() const { return _gids; }

	//
	// Overrides
	//
	virtual AnimateByGID* clone() const override;
	virtual AnimateByGID* reverse() const override;
	virtual void startWithTarget(Node *target) override;
	virtual void stop(void) override;
	virtual void update(float t) override;

CC_CONSTRUCTOR_ACCESS:
	AnimateByGID();
	virtual ~AnimateByGID();

	/** initializes the action with an ValueVector and will restore the original frame when the ValueVector is over */
	bool initWithGids(ValueVector gids, TMXLayer* layer, Vec2 site,float duration);

protected:
	std::vector<float>* _splitTimes;
	int             _nextFrame;
	SpriteFrame*    _origFrame;
	unsigned int    _executedLoops;
	ValueVector      _gids;
	TMXLayer*			_layer;
	Vec2			_site;

	EventCustom*    _frameDisplayedEvent;
private:
	CC_DISALLOW_COPY_AND_ASSIGN(AnimateByGID);
};

#endif /*__ANIMATEBYGID_H__*/