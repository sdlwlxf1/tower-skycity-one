#include "PopPanel.h"
#include "SimplePanel.h"
#include "Detect.h"

static PopPanel* ret = nullptr;

PopPanel::PopPanel()
{
	addChild(Layer::create(),0,0);
	addChild(Layer::create(),1,1);
	addChild(Layer::create(),2,2);
	addChild(Layer::create(),3,3);

	chatPanel = ChatPanel::create("chat");
	getChildByTag(0)->addChild(chatPanel);
	chatPanel->setEnabled(false);
}

PopPanel::~PopPanel()
{
	ret->release();
}

PopPanel* PopPanel::getInstance()
{
	if(ret == nullptr)
	{
		ret = new PopPanel();
		ret->autorelease();
		ret->retain();
	}
	return ret;
}

void PopPanel::reset()
{
	if(this->getParent() != nullptr)
	{
		this->removeFromParent();
	}
}

void PopPanel::onEnter()
{
	Layer::onEnter();
}

void PopPanel::onExit()
{
	Layer::onExit();
}

void PopPanel::addPanel(Layer* panel, int order)
{
	auto node = getChildByTag(order);
	if(node->getChildrenCount() > 0) return;
	node->addChild(panel);
	Detect::shareDetect()->setControlled(2, false);
}

SimplePanel* PopPanel::addPanelByName(std::string name, int order)
{
	SimplePanel *panel = nullptr;
	auto node = getChildByTag(order);
	if(node->getChildrenCount() > 0) return panel;
	panel = SimplePanel::create(name);
	node->addChild(panel);
	Detect::shareDetect()->setControlled(2, false);
	return panel;
}

void PopPanel::removeAllPanelsByOrder(int order)
{
	auto node = getChildByTag(order);
	node->removeAllChildren();
	check();
}

bool PopPanel::hasPanel(Layer* panel)
{
	for(auto node:this->getChildren())
	{
		auto childs = node->getChildren();
		if(childs.find(panel) != childs.end())
			return true;
	}
	return false;
}

Node* PopPanel::hasPanelByName(std::string name)
{
	for(auto node:this->getChildren())
	{
		for(auto child : node->getChildren())
		{
			if(child->getName() == name)
			{
				return child;
			}
		}
	}
	return nullptr;
}

void PopPanel::removePanel(Layer* panel)
{
	for(auto node:this->getChildren())
	{
		CC_SAFE_RETAIN(panel);
		node->removeChild(panel);
		check();
	}
}

void PopPanel::removePanelByName(std::string name)
{
	for(auto node:this->getChildren())
	{
		auto child = node->getChildByName(name);
		if(child != nullptr) 
		{
			CC_SAFE_RETAIN(child);
			node->removeChild(child);
			check();
		}
	}
}

void PopPanel::confirm(std::string name, std::string content, PromptPanel::buttonType type,std::function<void()> onDone,std::function<void()> onCancel, bool single, bool center)
{
	if(single == true)
	{
		if(this->getChildByTag(2)->getChildByName(name) != nullptr)
			return;
	}
	
	auto pan = PromptPanel::create("prompt");
	pan->initWithContent(content,type);
	pan->onDone = onDone;
	pan->onCancel = onCancel;
	this->getChildByTag(2)->addChild(pan,0,name);
	if(center == true)
		pan->setCenter();
	Detect::shareDetect()->setControlled(2, false);
}

void PopPanel::note(std::string name, std::string content, float delay, bool single, bool center)
{
	if(single == true)
	{
		if(this->getChildByTag(3)->getChildByName(name) != nullptr)
			return;
	}

	auto pan = PromptPanel::create("prompt");
	pan->initWithContent(content, PromptPanel::TYPE_NONE);
	pan->setSelfDestroy(delay);
	if(center == true)
		pan->setCenter();
	this->getChildByTag(3)->addChild(pan,0,name);
}

void PopPanel::check()
{
	bool b = true;
	for(auto node:this->getChildren())
	{
		if(node->getTag() == 3 || node->getTag() == 0) continue;
		if(node->getChildrenCount() > 0)
		{
			b = false;
			break;
		}
	}
	Detect::shareDetect()->setControlled(2, b);
}
