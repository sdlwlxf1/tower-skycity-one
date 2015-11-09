#include "DiaryPanel.h"
#include "scene/GameScene.h"
#include "UI/FilePanel.h"
#include "UI/ConfigPanel.h"
#include "utils/RichTextCreator.h"
#include "utils/PlotScript.h"


#define NONE_IMAGE "none.png"

DiaryPanel* DiaryPanel::create(std::string name, ValueMap model)
{
	DiaryPanel *pRet = new DiaryPanel(name, model);
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

DiaryPanel* DiaryPanel::create(std::string name)
{
	DiaryPanel *pRet = new DiaryPanel(name);
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

bool DiaryPanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(DiaryPanel::onButtonClicked, this));
	}
	_map = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_map"));
	_tip = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_tip"));
	_task = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_task"));

	_selected = _task;

	if(!_model["tasks"].isNull())
	{
		auto tasks = _model["tasks"].asValueMap();
		for(auto task:tasks)
		{
			addTask(task.first);
		}
	}

	if(!_model["tips"].isNull())
	{
		auto tips = _model["tips"].asValueMap();
		for(auto tip:tips)
		{
			addTip(tip.first);
		}
	}

	if(!_model["maps"].isNull())
	{
		auto maps = _model["maps"].asValueMap();
		for(auto map:maps)
		{
			addMap(map.first);
		}
	}

	_map->setVisible(false);
	_tip->setVisible(false);
	_task->setVisible(true);
	_selected = _task;

	return true;
}

const std::string DiaryPanel::getNameId()
{
	return _model["name"].asString();
}

bool DiaryPanel::onTrigger(EventCustom *event)
{
	//_curTower->onTrigger(event);
	auto data = static_cast<ValueMap*>(event->getUserData());
	if((*data)["type"].asString() == "check")
	{
	}
	return false;
}

std::string DiaryPanel::getType()
{
	return "Diary";
}

void DiaryPanel::initModel()
{

}

void DiaryPanel::setOwner(Actor* owner)
{
	_owner = owner;
}

Actor* DiaryPanel::getOwner()
{
	return _owner;
}

void DiaryPanel::show(std::string type, std::string name)
{
	Layout* selected;
	Node* content;
	if(type == "task")
	{	
		selected = _task;
	}
	else if(type == "tip")
	{
		selected = _tip;
	}
	else if(type == "map")
	{
		selected = _map;
	}

	content = selected->getChildByName(name);
	if(content == nullptr) return;
	_map->setVisible(false);
	_tip->setVisible(false);
	_task->setVisible(false);
	_selected = selected;
	_selected->setVisible(true);

	for(auto child:_selected->getChildren())
	{
		child->setVisible(false);
	}
	content->setVisible(true);
}

void DiaryPanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case EXIT:
			{
				destroy();
			}			
			break;
		case TASK:
			{
				_map->setVisible(false);
				_tip->setVisible(false);
				_task->setVisible(true);
				_selected = _task;
			}
			break;
		case TIP:
			{
				_map->setVisible(false);
				_task->setVisible(false);
				_tip->setVisible(true);
				_selected = _tip;
			}
			break;
		case MAP:
			{
				_tip->setVisible(false);
				_task->setVisible(false);
				_map->setVisible(true);
				_selected = _map;
			}
			break;
		case LEFT:
			{
				auto children = _selected->getChildren();
				auto size = children.size();
				if(size == 0) return;
				int next;
				int i = 0;
				for(auto node:children)
				{
					if(node->isVisible())
					{
						node->setVisible(false);
						break;
					}
					i++;
				}
				if(i == size)
				{
					next = 0;
				}
				else
				{
					next = --i;
					if(next == -1)
					{
						if(_selected != _task)
							next = size - 1;
						else
							next = 0;
					}
				}
				children.at(next)->setVisible(true);
			}
			break;
		case RIGHT:
			{
				auto children = _selected->getChildren();
				auto size = children.size();
				if(size == 0) return;
				int next;
				int i = 0;
				for(auto node:children)
				{
					if(node->isVisible())
					{
						node->setVisible(false);
						break;
					}
					i++;
				}
				// 没有找到
				if(i == size)
				{
					next = 0;
				}
				// 找到
				else
				{
					next = ++i;
					// 找到是最后一个
					if(next == size)
					{
						if(_selected != _task)
							next = 0;
						else
							next = size - 1;
					}
				}
				children.at(next)->setVisible(true);
			}
			break;
		}

		//_SAFE_RETAIN(this); 
		//this->removeFromParentAndCleanup(true);
	}
}

void DiaryPanel::addTask(std::string name)
{
	if(_tasks.find(name) != _tasks.end())
		return;
	auto content = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "diaryText",name.c_str());
	auto text = createRichTextWithStr(content);
	for(auto node:_task->getChildren())
	{
		if(node->isVisible())
		{
			node->setVisible(false);
		}
	}
	_task->addChild(text);
	text->setName(name);
	text->setContentSize(_task->getSize());
	text->ignoreContentAdaptWithSize(false);
	text->setVisible(true);
	_tasks[name] = cocos2d::Value(content);
}

void DiaryPanel::addMap(std::string name)
{
	if(_maps.find(name) != _maps.end())
		return;
	auto content = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "diaryText",name.c_str());
	RelativeLayoutParameter* lay = RelativeLayoutParameter::create();
	lay->setAlign(cocos2d::ui::RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT);

	auto image = ImageView::create(content,cocos2d::ui::Widget::TextureResType::PLIST);
	image->setLayoutParameter(lay);
	for(auto node:_map->getChildren())
	{
		if(node->isVisible())
		{
			node->setVisible(false);
		}
	}
	_map->addChild(image);
	image->setName(name);
	image->setVisible(true);
	_maps[name] = cocos2d::Value(content);
}

void DiaryPanel::addTip(std::string name)
{
	/*auto text = createRichTextWithStr(name);
	for(auto node:_tip->getChildren())
	{
		if(node->isVisible())
		{
			node->setVisible(false);
		}
	}
	_tip->addChild(text);
	text->setContentSize(_tip->getSize());
	text->ignoreContentAdaptWithSize(false);
	text->setVisible(true);
	_tips.push_back(cocos2d::Value(name));*/
	
	if(_tips.find(name) != _tips.end())
		return;
	auto content = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "diaryText",name.c_str());
	auto text = Label::create(content,"Arial",17,_tip->getSize());
	for(auto node:_tip->getChildren())
	{
		if(node->isVisible())
		{
			node->setVisible(false);
		}
	}
	_tip->addChild(text);
	text->setName(name);
	//text->updateContent();
	text->setAnchorPoint(Vec2(0,1));
	text->setColor(Color3B::BLACK);
	text->setVisible(true);
	_tips[name] = cocos2d::Value(content);
}

void DiaryPanel::onEnter()
{
	SimplePanel::onEnter();

	auto children = _task->getChildren();
	auto size = children.size();
	if(size == 0) return;
	for(auto node:children)
	{
		if(node->isVisible())
		{
			node->setVisible(false);
		}
	}
	children.at(size-1)->setVisible(true);
}

void DiaryPanel::onExit()
{
	SimplePanel::onExit();
}

void DiaryPanel::destroy()
{
	PopPanel::getInstance()->removePanel(this);
}

ValueMap DiaryPanel::saveModel()
{
	ValueMap model;
	model["tasks"] = _tasks;
	model["tips"] = _tips;
	model["maps"] = _maps;
	return model;
}

void DiaryPanel::loadModel(ValueMap &map)
{
	if(!map["tasks"].isNull())
	{
		_tasks = map["tasks"].asValueMap();
		for(auto task:_tasks)
		{
			addTask(task.first);
		}
	}

	if(!map["tips"].isNull())
	{
		_tips = map["tips"].asValueMap();
		for(auto tip:_tips)
		{
			addTask(tip.first);
		}
	}

	if(!map["maps"].isNull())
	{
		_maps = map["maps"].asValueMap();
		for(auto map:_maps)
		{
			addTask(map.first);
		}
	}
}
