#include "JniTest.h"
#include "ui/PopPanel.h"
#include "ui/PaperPanel.h"
#include "utils/PlotScript.h"
#include "utils/StringHandler.h"
#include "cell/CellFactory.h"
#include "Detect.h"
#include "ui/ShareAwardPanel.h"

using namespace CocosDenshion;

static JniTest* ret = nullptr;

JniTest* JniTest::getInstance()
{
	if(ret == nullptr)
	{
		ret = new JniTest();
		ret->autorelease();
		ret->retain();
	}
	return ret;
}

void JniTest::setPackageName(const char *packageName)
{
	CCLog("packageName: %s", packageName);	
}

void JniTest::exitApp()
{
	CCDirector::sharedDirector()->end();
}

void JniTest::shareSuccess(int index)
{
	Detect::shareDetect()->runAction(Sequence::create(DelayTime::create(1.0),CallFunc::create([=](){
		auto node = PopPanel::getInstance()->hasPanelByName("shareAward");
		if(node != nullptr)
		{
			auto user = Detect::shareDetect()->getUser();
			user->getShareReward(index);
		}
	}),nullptr));
}

void JniTest::shareFailed(int index)
{

}

void JniTest::orderSuccess(int goodId)
{
	CCLog("%d", goodId);
	Detect::shareDetect()->runAction(Sequence::create(DelayTime::create(1.0),CallFunc::create([=](){
		auto goodName = StringUtils::format("good%d",goodId+1);

		auto content = PlotScript::sharedHD()->getLuaVarString("script/Test.lua", goodName.c_str());
		ValueMap valueMap;
		stringToValueMap(std::string(content), valueMap);

		ValueMap model;
		model["typeId"] = valueMap["typeId"];
		auto good = CellFactory::getInstance()->createCell(model);

		if(model["typeId"].asInt() == 2003011)
		{
			auto effect = StringUtils::format("typeId:%d|num:%d",good->getTypeId(),valueMap["num"].asInt());
			EventHelper::getInstance()->dispatchByStr("target:player|type:getGemByType", effect);
		}
		else
		{
			auto effect = StringUtils::format("typeId:%d|num:%d",good->getTypeId(),valueMap["num"].asInt());
			EventHelper::getInstance()->dispatchByStr("target:player|type:addPropByType", effect);
		}
		log("success");
		PopPanel::getInstance()->note("cantEquip","type:Text---text:"+ a2u("¹§Ï²»ñµÃ ###") + "type:Text---text:"+ good->getNickName() + "|color:{255,0,0}");
	}),nullptr));
	
}

void JniTest::orderFailed(const char* info)
{
	/*std::string ii = std::string(info);
	PaperPanel* paper = PaperPanel::create("paper");
	paper->initWithText(ii);

	PopPanel::getInstance()->addPanel(paper,3);*/
	log("failed");
}
