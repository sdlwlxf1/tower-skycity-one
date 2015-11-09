#include "Prompt.h"
#include "utils/RichTextCreator.h"
#include "PopPanel.h"

#define NONE_IMAGE "none.png"

PromptPanel* PromptPanel::create(std::string name)
{
	PromptPanel *pRet = new PromptPanel(name);
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

bool PromptPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(PromptPanel::onButtonClicked, this));
	}
	_content = nullptr;
	_bg = static_cast<ImageView*>(Helper::seekWidgetByName(_root, "Image_bg"));
	_panelContent = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_content"));
	_enabled = true;
	_root->setTouchEnabled(true);
	return true;
}


void PromptPanel::setEnabled(bool bol)
{
	_enabled = bol;
	setKeyboardEnabled(bol);
	_root->setEnabled(bol);
	_root->setVisible(bol);
	/*if(bol == true)
	{
		Detect::shareDetect()->setControlled(false);
	}
	else
	{
		PopPanel::getInstance()->check();
	}*/
}

void PromptPanel::initWithContent(std::string contentStr, buttonType type)
{
	if(_content != nullptr) _content->removeFromParentAndCleanup(true);
	_content = createRichTextWithStr(contentStr);//
	_panelContent->addChild(_content);
	_content->ignoreContentAdaptWithSize(true);

	//_content->setContentSize(_panelContent->getSize());

	RelativeLayoutParameter* rp_LeftCenter = RelativeLayoutParameter::create();
	rp_LeftCenter->setAlign(RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT);
	_content->setLayoutParameter(rp_LeftCenter);
	//_content->setContentSize(_panelContent->getSize());
	//_bg->setSize(_content->getSize());
	_content->formatText();
	//_bg->setCapInsets(Rect(0,0,_content->getVirtualRendererSize().width,_content->getVirtualRendererSize().height));

	auto doneButton = static_cast<Button*>(_buttons->getChildByTag(buttons::DONE));
	auto cancelButton = static_cast<Button*>(_buttons->getChildByTag(buttons::CANCEL));
	doneButton->setEnabled(false);
	cancelButton->setEnabled(false);
	doneButton->setVisible(false);
	cancelButton->setVisible(false);
	switch (type)
	{
	case PromptPanel::buttonType::TYPE_DONE:
		{
			_panelContent->setPositionY(_panelContent->getPositionY() + 10);
			doneButton->setVisible(true);
			doneButton->setEnabled(true);
			//doneButton->setPosition(Vec2(0,-10));
		}
		break;
	case PromptPanel::buttonType::TYPE_DONE_CANCEL:
		{
			_panelContent->setPositionY(_panelContent->getPositionY() + 10);
			doneButton->setEnabled(true);
			cancelButton->setEnabled(true);
			doneButton->setVisible(true);
			cancelButton->setVisible(true);
			//doneButton->setPosition(Vec2(100,-10));
			//cancelButton->setPosition(Vec2(-100,-10));
		}
		break;
	case PromptPanel::buttonType::TYPE_NONE:
		{
			_buttons->setTouchEnabled(false);
			_panelContent->setTouchEnabled(false);
			_root->setTouchEnabled(false);
			doneButton->setEnabled(false);
			cancelButton->setEnabled(false);
			_bg->setSize(Size(_content->getVirtualRendererSize().width + 60,_content->getVirtualRendererSize().height + 60));
			//_bg->setVisible(false);
		}
		break;
	default:
		break;
	}
	_container->setSize(_bg->getSize());
	_container->setAnchorPoint(Vec2(0,0));

	_container->setScale(0.8f);
	_container->setOpacity(200);
	_container->runAction(FadeIn::create(0.2));
	_container->runAction(ScaleTo::create(0.2,1.0f,1.0f,1.0f));
}

void PromptPanel::setCenter()
{
	auto size = Director::getInstance()->getVisibleSize();
	_container->setPositionY(size.height / 2);
}

void PromptPanel::setSelfDestroy(float delay)
{
	this->runAction(Sequence::create(DelayTime::create(delay),CallFunc::create([=](){CC_SAFE_RETAIN(this); 
	this->removeFromParentAndCleanup(true);}),nullptr));
}

void PromptPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case DONE:
			{
				if(onDone)
				{
					onDone();
				}
				destroy();
			}
			break;
		case CANCEL:
			{
				if(onCancel)
				{
					onCancel();
				}
				destroy();
			}
			break;
		}

		//_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}