#include "ChatPanel.h"
#include "mutual/ChatManager.h"
#include "PopPanel.h"
#include "utils/RichTextCreator.h"
#include "Detect.h"

#define NONE_IMAGE "none.png"

ChatPanel* ChatPanel::create(std::string name)
{
	ChatPanel *pRet = new ChatPanel(name);
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

bool ChatPanel::init()
{
	_content = nullptr;
	_layerHead = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Layer_head"));
	_buttonTouch = static_cast<Button*>(Helper::seekWidgetByName(_root, "Button_touch"));
	_buttonTouchLayout = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_begin_touch"));
	_panelTouch = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_touch"));
	_panelContentLeft = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_content_left"));
	_panelContentRight = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_content_right"));
	_panelButtonLeft = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_button_left"));
	_panelButtonRight = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_button_right"));
	_panelImageLeft = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_image_left"));
	_panelImageRight = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_image_right"));
	_panelContentLarge = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_content_large"));

	_panelButtonTouch = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_button_touch"));

	_btnTouchSprite = Sprite::createWithSpriteFrameName("chat/buttonTouch/1.png");
	Animation* animation = Animation::create();  
	for( int i=1;i<=3;i++)  
	{  
		std::string name = "chat/buttonTouch/" + cocos2d::Value(i).asString() + ".png";
		auto spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		animation->addSpriteFrame(spriteFrame); //加载动画的帧  
	}  
	animation->setDelayPerUnit(0.6f / 3.0f);  
	animation->setLoops(1); //动画循环10次
	Animate *animate = Animate::create(animation);
	_btnTouchAction = RepeatForever::create(animate);
	_btnTouchAction->retain();
	
	_buttonTouchLayout->addChild(_btnTouchSprite);

	_headBg = ImageView::create();
	_headFace = ImageView::create();
	_layerHead->addChild(_headBg);
	_layerHead->addChild(_headFace);

	_buttonTouch->addTouchEventListener(CC_CALLBACK_2(ChatPanel::onButtonClicked, this));
	_panelTouch->addTouchEventListener(CC_CALLBACK_2(ChatPanel::onButtonClicked, this));
	_panelButtonTouch->addTouchEventListener(CC_CALLBACK_2(ChatPanel::onButtonClicked, this));

	return true;
}

void ChatPanel::initWithChat(ChatUnit* chat)
{
	//_content->setString(chat->content);
	if(_content != nullptr) _content->removeFromParentAndCleanup(true);
	_content = createRichTextWithStr(chat->content);//

	if(chat->begin == 1)
	{
		_panelTouch->setTouchEnabled(false);
		_buttonTouchLayout->setVisible(true);
		Detect::shareDetect()->setControlled(1,false);

		_root->setOpacity(0);
		_root->runAction(FadeIn::create(0.3));
	}
	else
	{
		_panelTouch->setTouchEnabled(true);
		_buttonTouchLayout->setVisible(false);
	}

	_buttonTouchLayout->retain();
	_buttonTouchLayout->removeFromParent();

	_layerHead->retain();
	_layerHead->removeFromParent();

	if(chat->actorId == 0)
	{
		_headBg->loadTexture(NONE_IMAGE,Widget::TextureResType::PLIST);
		_headFace->loadTexture(NONE_IMAGE,Widget::TextureResType::PLIST);
		_panelContentLarge->addChild(_content);
		_panelButtonRight->addChild(_buttonTouchLayout);
		_btnTouchSprite->runAction(_btnTouchAction);

		_content->setContentSize(_panelContentLarge->getSize());
	}
	else
	{
		_headBg->loadTexture(StringUtils::format("actor%d/actor_%d.png",chat->actorId,chat->actorId,chat->imageId),Widget::TextureResType::PLIST);
		if(chat->imageId == 0)
		{
			_headFace->loadTexture(NONE_IMAGE,Widget::TextureResType::PLIST);
		}
		else
		{
			_headFace->loadTexture(StringUtils::format("actor%d/actor_%d_%d.png",chat->actorId,chat->actorId,chat->imageId),Widget::TextureResType::PLIST);
		}

		if(chat->alignType == "left")
		{
			_panelContentLeft->addChild(_content);
			_panelImageRight->addChild(_layerHead);
			_panelButtonLeft->addChild(_buttonTouchLayout);
			_btnTouchSprite->runAction(_btnTouchAction);

			_content->setContentSize(_panelContentLeft->getSize());
		}
		else if(chat->alignType == "right")
		{
			_panelContentRight->addChild(_content);
			_panelImageLeft->addChild(_layerHead);
			_panelButtonRight->addChild(_buttonTouchLayout);
			_btnTouchSprite->runAction(_btnTouchAction);

			_content->setContentSize(_panelContentRight->getSize());
		}
	}
	_content->ignoreContentAdaptWithSize(false);

	_startDelay = false;
	this->runAction(Sequence::create(DelayTime::create(0.5),CallFunc::create([=](){_startDelay = true;}),nullptr));
}

void ChatPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if(touchType == Widget::TouchEventType::BEGAN)
	{
		if(_startDelay == true)
			ChatManager::getInstance()->nextChat();
	}
}

void ChatPanel::setEnabled(bool bol)
{
	_enabled = bol;
	setKeyboardEnabled(bol);
	_root->setVisible(bol);
	_root->setTouchEnabled(bol);
	_panelTouch->setTouchEnabled(bol);
}

void ChatPanel::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		ChatManager::getInstance()->nextChat();
	}
}
