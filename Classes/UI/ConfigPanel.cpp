#include "ConfigPanel.h"
#include "Detect.h"

#define NONE_IMAGE "none.png"

ConfigPanel* ConfigPanel::create(std::string name)
{
	ConfigPanel *pRet = new ConfigPanel(name);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool ConfigPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(ConfigPanel::onButtonClicked, this));
	}
	_root->setTouchEnabled(true);

	_soundOffImg = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_sound_off"));
	_soundOnImg = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_sound_on"));
	_effectOnImg = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_shake_on"));
	_shakeOffImg = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_shake_off"));

	auto soundOn = Detect::shareDetect()->getSoundOn();
	auto effectOn = Detect::shareDetect()->getEffectOn();
	_soundOnImg->setVisible(soundOn);
	_soundOffImg->setVisible(!soundOn);
	_effectOnImg->setVisible(effectOn);
	_shakeOffImg->setVisible(!effectOn);

	return true;
}

void ConfigPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case BG:
			{
				auto soundOn = Detect::shareDetect()->getSoundOn();
				soundOn = !soundOn;
				Detect::shareDetect()->setSoundOn(soundOn);
				_soundOnImg->setVisible(soundOn);
				_soundOffImg->setVisible(!soundOn);
				
				
			}	
			break;
		case EFFECT:
			{
				auto effectOn = Detect::shareDetect()->getEffectOn();
				effectOn = !effectOn;
				Detect::shareDetect()->setEffectOn(effectOn);
				_effectOnImg->setVisible(effectOn);
				_shakeOffImg->setVisible(!effectOn);
			}
			break;
		case CLOSE:
			{
				CC_SAFE_RETAIN(this); 
				this->removeFromParentAndCleanup(true);
			}
			break;
		}
	}
}