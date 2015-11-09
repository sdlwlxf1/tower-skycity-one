#include "FilePanel.h"
#include "utils/StringHandler.h"
#include "model/Model.h"
#include "scene/GameScene.h"
#include "scene/PreScene.h"
#include "UI/ConfigPanel.h"
#include "Detect.h"
#include "utils/PlotScript.h"
#include "utils/Tools.h"

#define NONE_IMAGE "none.png"

FilePanel* FilePanel::create(std::string name)
{
	FilePanel *pRet = new FilePanel(name);
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

bool FilePanel::init()
{
	for(auto node:_buttons->getChildren())
	{
		auto button = static_cast<Button*>(node);
		button->addTouchEventListener(CC_CALLBACK_2(FilePanel::onButtonClicked, this));
	}
	_root->setTouchEnabled(true);

	auto unitsPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_units"));
	auto unitPan = static_cast<Layout*>(Helper::seekWidgetByName(_root, "Panel_unit"));
	unitPan->retain();
	unitPan->removeFromParent();

	auto saveNum = Configuration::getInstance()->getValue("saveNum").asInt();
	for(int i = 0; i < saveNum; i++)
	{
		auto fileUnit = FileUnit::create(i);
		auto pan = unitPan->clone();
		pan->setTag(i);
		unitsPan->addChild(pan);
		pan->setPosition(Vec2(pan->getPosition().x, pan->getPosition().y - 70 * i));
		fileUnit->setView(pan);
		_units.pushBack(fileUnit);
	}
	unitPan->release();

	for(auto node:unitsPan->getChildren())
	{
		auto pan = static_cast<Layout*>(node);
		pan->addTouchEventListener(CC_CALLBACK_2(FilePanel::onFileUnitsClicked, this));
	}

	setType(SAVE);
	return true;
}

void FilePanel::setType(func type)
{
	_type = type;
	switch (type)
	{
	case FilePanel::SAVE:
		{

		}
		break;
	case FilePanel::LOAD:
		{

		}
		break;
	default:
		break;
	}
}

void FilePanel::onFileUnitsClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto pan = static_cast<Layout*>(ref);
		auto tag = pan->getTag();
		if(_type == FilePanel::SAVE)
		{
			_units.at(tag)->save();
		}
		else if(_type == FilePanel::LOAD)
		{
			if(_units.at(tag)->getIsExist() == false)
			{
				PopPanel::getInstance()->note("noFile","type:Text---text:"+ a2u("空存档"),1.0f,false,true);
				return;
			}
			PopPanel::getInstance()->confirm("saveConfirm","type:Text---text:"+ a2u("确定要读取存档？"),PromptPanel::TYPE_DONE_CANCEL,[=](){
				destroy();
				_units.at(tag)->load();
			},nullptr,true,true);
		}
	}
}

void FilePanel::onButtonClicked(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED) 
	{
		auto button = static_cast<Button*>(ref);
		auto tag = button->getTag();
		switch(tag)
		{
		case CLOSE:
			{
				destroy();
			}
			break;
		case RESTART:
			{
				PopPanel::getInstance()->confirm("prompt","type:Text---text:"+ a2u("确定要重新开始游戏吗？"),PromptPanel::TYPE_DONE_CANCEL,[=](){
					Detect::shareDetect()->load = false;
					auto gameScene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
					if(gameScene != nullptr)
					{
						gameScene->resetInstance();
						auto scene = GameScene::create();
						Director::getInstance()->replaceScene(scene);
					}
					else
					{
						auto s = GameScene::create();
						auto scene = TransitionFade::create(1.0f, s, Color3B::BLACK);
						if (scene)
						{
							Director::getInstance()->replaceScene(scene);
						}
					}
				},nullptr,true,true);
				destroy();
			}
			break;
		}
	}
}

void FilePanel::onEnter()
{
	SimplePanel::onEnter();
}

FileUnit* FileUnit::create(int id)
{
	FileUnit *pRet = new FileUnit(id);
	if (pRet)
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

FileUnit::FileUnit(int id)
{
	_id = id;
	_path = FileUtils::sharedFileUtils()->getWritablePath() + "save" + cocos2d::Value(id+1).asString() + ".plist";
	//FileUtils::getInstance()->isFileExist(path);
}

void FileUnit::setView(Widget* view)
{
	_view = view;
	updateView();
}

void FileUnit::updateView()
{
	auto info = static_cast<Text*>(Helper::seekWidgetByName(_view, "Label_info"));
	auto date = static_cast<Text*>(Helper::seekWidgetByName(_view, "Label_date"));

	auto loadMap = FileUtils::getInstance()->getValueMapFromFile(_path);
	if(!loadMap["exist"].isNull() && loadMap["exist"].asBool() == true)
	{
		auto loadMap = FileUtils::getInstance()->getValueMapFromFile(_path);
		auto map = loadMap["label"].asValueMap();
		auto infoStr = map["info"].asString();
		auto dateStr = map["date"].asString();
		info->setString(infoStr);
		date->setString(dateStr);
		_isExist = true;
	}
	else
	{
		info->setString(a2u("空存档"));
		date->setString("");
		_isExist = false;
	}
}

void FileUnit::save()
{
	auto loadMap = FileUtils::getInstance()->getValueMapFromFile(_path);
	if(!loadMap["exist"].isNull() && loadMap["exist"].asBool() == true)
	{
		PopPanel::getInstance()->confirm("saveConfirm","type:Text---text:"+ a2u("确定要覆盖当前存档吗？"),PromptPanel::TYPE_DONE_CANCEL,[=](){
			saveModel();
		},nullptr,true,true);
	}
	else
	{
		saveModel();
	}
	_view->runAction(Sequence::create(DelayTime::create(1),CallFunc::create([=](){updateView();}),nullptr));
}

void FileUnit::saveModel()
{
	auto map = Detect::shareDetect()->saveModel();
	ValueMap labelMap;
	auto player = Detect::shareDetect()->getPlayer();
	std::string info = StringUtils::format("%s %d %d %d %d %d",player->getNickName().c_str(),player->getHp(),player->getStr(),player->getDef(),player->getGold(),player->getXp());
	labelMap["info"] = info;
	std::string date = getTimeStr();
	labelMap["date"] = date;
	map["label"] = labelMap;
	map["exist"] = true;
	FileUtils::getInstance()->writeToFile(map,_path);
}

void FileUnit::load()
{
	auto scene = Director::getInstance()->getRunningScene();
	if(dynamic_cast<GameScene*>(scene) != nullptr)
	{
		EventHelper::getInstance()->dispatchByStr("target:hud|type:curtain", "action:fadeIn|delay:0.5");
		Detect::shareDetect()->runAction(Sequence::create(DelayTime::create(0.5),CallFunc::create([=](){
			auto loadMap = FileUtils::getInstance()->getValueMapFromFile(_path);
			Detect::shareDetect()->loadModel(loadMap);
			EventHelper::getInstance()->dispatchByStr("target:hud|type:curtain", "action:fadeOut|delay:1");
		}),nullptr));
	}
	else
	{
		Detect::shareDetect()->load = true;
		Detect::shareDetect()->loadPath = _path;

		auto s = GameScene::create();
		auto scene = TransitionFade::create(1.0f, s, Color3B::BLACK);
		if (scene)
		{
			Director::getInstance()->replaceScene(scene);
		}
	}
}

void FileUnit::clear()
{
	auto loadMap = FileUtils::getInstance()->getValueMapFromFile(_path);
	loadMap["exist"] = false;
	FileUtils::getInstance()->writeToFile(loadMap,_path);
}