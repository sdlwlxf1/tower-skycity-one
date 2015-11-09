#include "AnimateByGID.h"


USING_NS_CC;
//
// AnimateByGID
//
AnimateByGID* AnimateByGID::create(ValueVector gids, TMXLayer* layer, Vec2 site,float duration)
{
    AnimateByGID *animate = new AnimateByGID();
    animate->initWithGids(gids, layer, site, duration);
    animate->autorelease();
    return animate;
}

AnimateByGID::AnimateByGID()
: _splitTimes(new std::vector<float>)
, _nextFrame(0)
, _origFrame(nullptr)
, _executedLoops(0)
, _gids(ValueVector())
, _frameDisplayedEvent(nullptr)
{

}

AnimateByGID::~AnimateByGID()
{
    //_SAFE_RELEASE(_gids);
	CC_SAFE_RELEASE(_origFrame);
    CC_SAFE_DELETE(_splitTimes);
    CC_SAFE_RELEASE(_frameDisplayedEvent);
}

bool AnimateByGID::initWithGids(ValueVector gids, TMXLayer* layer, Vec2 site, float duration)
{
    //ASSERT( gids!=ValueVector::, "AnimateByGID: argument ValueVector must be non-nullptr");

    //float singleDuration = gids->getDuration();
	_layer = layer;
	_site = site;

	float singleDuration = duration * gids.size();

    if (ActionInterval::initWithDuration(singleDuration))
    {
        _nextFrame = 0;
        setGids(gids);
        _origFrame = nullptr;
        _executedLoops = 0;

        _splitTimes->reserve(gids.size());

        float aumUnitsOfTime = 0;
        float newUnitOfTimeValue = singleDuration / gids.size();

        for (auto frame : gids)
        {
            float value = (aumUnitsOfTime * newUnitOfTimeValue) / singleDuration;
            aumUnitsOfTime += 1.0f;
            _splitTimes->push_back(value);
        }    
        return true;
    }
    return false;
}

void AnimateByGID::setGids(ValueVector gids)
{
    if (_gids != gids)
    {
        //_SAFE_RETAIN(gids);
        //_SAFE_RELEASE(_gids);
        _gids = gids;
    }
}

AnimateByGID* AnimateByGID::clone() const
{
	// no copy constructor
	auto a = new AnimateByGID();
	a->initWithGids(ValueVector(_gids),_layer,_site,_duration);
	a->autorelease();
	return a;
}

void AnimateByGID::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    Sprite *sprite = static_cast<Sprite*>(target);

    //_SAFE_RELEASE(_origFrame);

	/*if (_gids->getRestoreOriginalFrame())
	{
	_origFrame = sprite->getSpriteFrame();
	_origFrame->retain();
	}*/
    _nextFrame = 0;
    _executedLoops = 0;
}

void AnimateByGID::stop()
{
	/*if (_gids->getRestoreOriginalFrame() && _target)
	{
	static_cast<Sprite*>(_target)->setSpriteFrame(_origFrame);
	}*/

    ActionInterval::stop();
}

void AnimateByGID::update(float t)
{
    // if t==1, ignore. ValueVector should finish with t==1
    if( t < 1.0f ) {
        t *= 1;

        // new loop?  If so, reset frame counter
        unsigned int loopNumber = (unsigned int)t;
        if( loopNumber > _executedLoops ) {
            _nextFrame = 0;
            _executedLoops++;
        }

        // new t for gids
        t = fmodf(t, 1.0f);
    }

    auto numberOfFrames = _gids.size();

    for( int i=_nextFrame; i < numberOfFrames; i++ ) {
        float splitTime = _splitTimes->at(i);

        if( splitTime <= t ) {
            _layer->setTileGID(_gids.at(i).asInt(),_site);
            _nextFrame = i+1;
        }
        // Issue 1438. Could be more than one frame per tick, due to low frame rate or frame delta < 1/FPS
        else {
            break;
        }
    }
}

AnimateByGID* AnimateByGID::reverse() const
{
    //auto oldArray = _gids->getFrames();
    //Vector<ValueVectorFrame*> newArray(oldArray.size());
   
    //if (oldArray.size() > 0)
    //{
    //    for (auto iter = oldArray.crbegin(); iter != oldArray.crend(); ++iter)
    //    {
    //        ValueVectorFrame* animFrame = *iter;
    //        if (!animFrame)
    //        {
    //            break;
    //        }

    //        newArray.pushBack(animFrame->clone());
    //    }
    //}

    ValueVector newVec = _gids;
    return AnimateByGID::create(newVec, _layer, _site, _duration);
}