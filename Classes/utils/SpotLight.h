#include "cocos2d.h"
USING_NS_CC;
class SpotLight:public Sprite
{
private:
	float originRadius;
	int symbolFlag;
public:
	RenderTexture * renderTexture;
	Color4F renderColor;
	float spotLightRadius;
public:
	static SpotLight * create(RenderTexture * texture,float radius,Color4F color);
	bool init(RenderTexture * texture,float radius,Color4F color);
	virtual void onEnterTransitionDidFinish();
	virtual void update(float delta);
};