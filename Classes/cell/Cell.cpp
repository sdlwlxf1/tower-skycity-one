#include "Cell.h"
#include "Global.h"
#include "Detect.h"

bool Cell::onTrigger(EventCustom *event)
{
	auto data = static_cast<ValueMap*>(event->getUserData());
	auto effect = (*data)["effect"].asValueMap();
	for(auto type:_triggerOn)
	{
		if((*data)["type"].asString() == type.first)
		{
			for(auto tri:type.second.asValueVector())
			{
				auto vec = tri.asValueVector();
				ValueMap map;
				stringToValueMap(vec.at(0).asString(),map);
				if(map["target"].isNull())
				{
					map["target"] = (*data)["senderName"].asString();
				}
				else if(map["target"].asString() == "this")
				{
					map["target"] = this->getName();
				}
				else if(map["target"].asString() == "player")
				{
					map["target"] = Detect::shareDetect()->getPlayer()->getName();
				}
				auto mapStr = valueMapToString(map);
				if(!map["delay"].isNull())
				{
					auto delay = map["delay"].asFloat();
					Director::getInstance()->getRunningScene()->runAction(Sequence::create(DelayTime::create(delay),CallFunc::create([=](){
						EventHelper::getInstance()->dispatchByStr(mapStr,vec.at(1).asString());
					}),nullptr));
				}
				else
				{
					EventHelper::getInstance()->dispatchByStr(mapStr,vec.at(1).asString());
				}
			}
		}
	}
	return false;
}

#if DETECT_ACTOR == 1
void Cell::setLabel(std::string str1, std::string str2)
{
	_label1->setString(str1);
	//_label2->setString(win);
	if(str2 == "win")
	{
		//_label1->enableShadow(Color4B::GREEN,Size(1,-1),100);
		_label1->enableOutline(Color4B(120,196,0,255),1);

	}
	else
	{
		//_label1->enableShadow(Color4B::RED,Size(1,-1),100);
		_label1->enableOutline(Color4B(196,0,0,255),1);
	}
}
#endif

Vec2 Cell::getSite() const
{
	return _site;
}

void Cell::setSite(const Vec2 &vec)
{
	setPosition(Vec2(vec.x * TILE_WIDTH + TILE_HALF_WIDTH, vec.y * TILE_HEIGHT + TILE_HALF_HEIGHT));
	if(vec != _site)
	{
		_lastSite = _site;
		_site = vec;
		moveToNewSite();
	}
}

void Cell::detachFromUnit()
{
	if(_unit)
	{
		_unit->detach(this);
	}
}

void Cell::removeFromFloor()
{
	if(_floor)
	{
		_floor->removeCell(this);
	}
}

void Cell::updateSite()
{
	if(_lastSite != _site)
	{
		moveToNewSite();
	}
	stepToNewSite();
}

void Cell::moveToNewSite()
{
	if(_floor == nullptr) return;
	_floor->attachToUnit(this, _site);
	//EventHelper::getInstance()->dispatchByStr("target:"+getFloor()->getName()+"|type:crashOn|senderType:Role|senderName:"+_model["name"].asString(),"newSite:" + StringUtils::format("{%f,%f}",_site.x,_site.y));
	_floor->detachFromUnit(this,_lastSite);
	//EventHelper::getInstance()->dispatchByStr("target:"+getFloor()->getName()+"|type:crashOut|senderType:Role|senderName:"+_model["name"].asString(),"lastSite:" + StringUtils::format("{%f,%f}",_lastSite.x,_lastSite.y));
}

void Cell::stepToNewSite()
{
	if(_floor == nullptr) return;
	_floor->centerToUnit(this,_site);
	/*EventHelper::getInstance()->dispatchByStr("target:"+getFloor()->getName()+"|type:stepOn|senderType:Role|senderName:"+_model["name"].asString(),"newSite:" + StringUtils::format("{%f,%f}",_site.x,_site.y));*/
}

int Cell::getLayer()
{
	return _model["layer"].asInt();
}

void Cell::setLayer(int id)
{
	_layer = id;
}

int Cell::isBlock()
{
	return 1;
}

bool Cell::init()
{
	Layer::init();
	bool bRet = false;
	do{
		initNull();
		initModel();
		initView();
		initOthers();
		
		// 事件监听
		_listener = EventListenerCustom::create(getName(), [=](EventCustom *event){onTrigger(event);});
		_eventDispatcher->addEventListenerWithFixedPriority(_listener, 1);

		bRet = true;
	} while (0);
	return bRet;
}

void Cell::initNull()
{
	_curAnimate = nullptr;
	_curAnimation = nullptr;
	_lastSite = _site = Vec2(-1,-1);
	_shader = nullptr;
	_floor = nullptr;
	_refresh = false;
	_busy = false;
	_mutual = 0;
}

void Cell::destroy()
{
	//this->stopAllActions();
	/*_eventDispatcher->removeEventListener(_listener);
	CC_SAFE_RELEASE(_listener);
	_listener = nullptr;*/
	//_curAnimate = nullptr;
	//_curAnimation = nullptr;
	//_blockFunc = nullptr;
	//removeFromParent();
}

void Cell::initModel()
{
	
}

void Cell::loadModel(ValueMap &map)
{
	Model::modelCoverModel(map,_model);
}

void Cell::refreshModel()
{

}

void Cell::refresh()
{
	_refresh = true;
	refreshModel();
	initOthers();
	_refresh = false;
}

ValueMap Cell::saveModel()
{
	ValueMap model;
	model["name"] = getName();
	model["site"] = pointToString(getSite());
	model["typeId"] = getTypeId();
	model["trigger"] = _model["trigger"];
	model["busy"] = _busy;
	model["mutual"] = _mutual;
	return model;
}

void Cell::initView()
{
	_view = Sprite::createWithSpriteFrameName(_model["image"].asString());
	_view->setAnchorPoint(Point(0.5,0.25));
	addChild(_view);

#if DETECT_ACTOR == 1
	_label1 = Label::createWithTTF("", "fonts/58983.ttf", 13);
	_label1->setColor(Color3B::WHITE);
	addChild(_label1);
	_label1->setPosition(Vec2(30,10));
	_label1->setAnchorPoint(Vec2(0.5, 0.5));
	_label1->setVisible(true);
	_label1->setString("");
	//_label1->enableOutline(Color4B::GREEN,10);
	/*_label2 = Text::create("","Arial",15);
	_label2->setColor(Color3B::WHITE);
	addChild(_label2);
	_label2->setPosition(Vec2(30,10));
	_label2->setAnchorPoint(Vec2(0.5, 0.5));
	_label2->setVisible(false);
	_label2->setString("");
	_label2->enableShadow(Color4B::RED,Size(1,-1),100);*/
#endif
	//setLabel(getName(),"");
}

void Cell::initOthers()
{
	if(!_model["busy"].isNull())
	{
		_busy = _model["busy"].asBool();
	}
	if(!_model["mutual"].isNull())
	{
		_mutual = _model["mutual"].asInt();
	}
	// 脚本初始化
	if(!_model["trigger"].isNull() && _model["trigger"].asString() != "")
	{
		ValueVector vec1;
		stringToValueVector(_model["trigger"].asString(),"###",vec1);
		for (auto i:vec1)
		{
			strArray a;
			split(i.asString(),"$",a);

			if(a.size() > 1)
			{
				ValueVector vec3;
				stringToValueVector(a.at(1),"~~~",vec3);
				if(vec3.size() < 2)
				{
					vec3.push_back(cocos2d::Value(""));
				}
				if(_triggerOn[a.at(0)].isNull())
				{
					_triggerOn[a.at(0)] = ValueVector();
				}
				_triggerOn[a.at(0)].asValueVector().push_back(cocos2d::Value(vec3));
			}			
		}
	}
}

Cell::~Cell()
{
	_eventDispatcher->removeEventListener(_listener);
}

void Cell::setBlockFunc(std::function<int(Vec2&)> func)
{
	_blockFunc = func;
}

const std::string Cell::getNameId()
{
	return _model["name"].asString();
}

int Cell::getTypeId()
{
	return _model["typeId"].asInt();
}

std::string Cell::getEffect()
{
	return _model["effect"].asString();
}

std::string Cell::getImageUrl()
{
	return _model["image"].asString();
}

const cocos2d::Value Cell::getModelByName(std::string name)
{
	return _model[name];
}

void Cell::setModelByName(std::string name, cocos2d::Value value)
{
	_model[name] = value;
}

void Cell::onEnter()
{
	Layer::onEnter();
	if(_shader != nullptr)
	{
		_shader->shade->runAction(RepeatForever::create(Sequence::create(FadeTo::create(0.1,80),FadeTo::create(0.1,200),DelayTime::create(0.8),nullptr)));
		//_shader->shade->runAction(RepeatForever::create(Shaky3D::create(5, Size(15,10), 4, false)));
	}
}

void Cell::onExit()
{
	Layer::onExit();
	//_eventDispatcher->removeEventListener(_listener);
}

void Cell::disappear()
{
	this->setBusy(true);
	_view->runAction(Sequence::create(FadeOut::create(1.0f),CallFunc::create([=](){this->setBusy(false);this->removeFromFloor();}),nullptr));
}

std::string Cell::getType()
{
	return "Cell";
}

std::string Cell::getNickName()
{
	return _model["nickName"].asString();
}

bool Cell::setAnimation(const std::string &str, bool loop, CallFuncN* callBack, bool again, float duration)
{
	if(_animations.find(str) == _animations.end())
		return false;
	auto animation = AnimationCache::getInstance()->getAnimation(_animations[str].asString());
	if (again == false && _curAnimation == animation)
		return false;
	_curAnimation = animation;
	if (_curAnimate != nullptr)
	{
		_view->stopAction(_curAnimate);
	}
	auto animate = Animate::create(animation);
	if(duration != -1) animate->setDuration(duration);
	if(loop == true)
	{
		_curAnimate = RepeatForever::create(Sequence::create(animate,callBack,NULL));
	}
	else
	{
		_curAnimate = Sequence::create(animate,callBack,NULL);
	}
	_view->runAction(_curAnimate);
	return true;
}

Shader* Shader::create(std::string pshade, std::string pmask)
{
	Shader *ret = new Shader(pshade, pmask);
	if (ret && ret->init())
	{
		//ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

Shader::Shader(std::string pshade, std::string pmask)
{
	shade = Sprite::createWithSpriteFrameName(pshade);
	mask = Sprite::createWithSpriteFrameName(pmask);
	//BlendFunc func1 = {GL_ZERO, GL_SRC_ALPHA};
	//shade->setBlendFunc(func1);
	
	shade->retain();
	mask->retain();
}

Shader::~Shader()
{
	shade->release();
	mask->release();
}

bool Shader::init()
{
	return true;
}

void Shader::update(Vec2 pos)
{
	shade->setPosition(pos);
	mask->setPosition(pos);
}

