#include "Swap.h"
#include "utils/StringHandler.h"
#include "UI/SwapPanel.h"
#include "Detect.h"
#include "cell/CellFactory.h"
#include "utils/PlotScript.h"
#include "ui/PopPanel.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;

Swap::Swap(std::string buyer, std::string seller)
{
	_buyer = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(buyer));
	_seller = dynamic_cast<Actor*>(Detect::shareDetect()->getCellByName(seller));

	auto swapId = _seller->getModelByName("swapId").asInt();
	int swapCount = _seller->getSwapCount();
	//shopId = 1;
	std::string shopStr;
	std::string preCondtion;
	std::string swapName = "swap" + cocos2d::Value(swapId).asString() + "_" + cocos2d::Value(swapCount).asString();
	std::string swapNumStr = "swap" + cocos2d::Value(swapId).asString() + "_num";
	int swapNum = cocos2d::Value(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", swapNumStr.c_str())).asInt();
	if(swapNum == 0) swapNum = 1;

	bool canSwap = true;

	while(true)
	{
		if(swapCount >= swapNum)
		{
			PopPanel::getInstance()->note("cannt","type:Text---text:"+ a2u("超过交易上限，无法使用"), 1.0f, true, true);
			canSwap = false;
			break;
		}

		shopStr = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", swapName.c_str(),"content");
		title = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", swapName.c_str(),"title");
		preCondtion = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", swapName.c_str(),"preCondition");
		if(preCondtion != "")
		{
			ValueMap preVec;
			stringToValueMap(preCondtion,preVec);
			for (auto pair:preVec)
			{
				auto num = pair.second.asInt();
				if(pair.first == "level")
				{
					if(_buyer->getLevel() > num)
					{
						PopPanel::getInstance()->note("levelBeyond","type:Text---text:"+ a2u("等级过高，无法使用"), 1.0f, true, true);
						canSwap = false;
						break;
					}
				}
				else if(pair.first == "")
				{

				}
				else if(pair.first == "")
				{

				}
				else if(pair.first == "")
				{

				}
				else if(pair.first == "")
				{

				}
			}
		}
		break;
	}
	

	if(canSwap == false)
	{
		this->removeFromParent();
		return;
	}

	if(shopStr != "")
	{
		ValueVector vec1;
		stringToValueVector(shopStr,"###",vec1);
		for (auto i:vec1)
		{
			ValueVector vec2;
			stringToValueVector(i.asString(),"~~~",vec2);
			ValueVector option;
			for(auto j:vec2)
			{
				ValueVector vec3;
				ValueVector vecout;
				stringToValueVector(j.asString(),"$$$",vec3);
				for(auto k:vec3)
				{
					ValueMap map;
					stringToValueMap(k.asString(), map);
					vecout.push_back(cocos2d::Value(map));
				}
				option.push_back(cocos2d::Value(vecout));
			}
			options.push_back(cocos2d::Value(option));
		}
	}
	_swapPanel = SwapPanel::create("swap");
	_swapPanel->build(this);
	PopPanel::getInstance()->addPanel(_swapPanel,1);
}

Swap* Swap::create(std::string buyer, std::string seller)
{
	Swap *pRet = new Swap(buyer, seller);
	if (pRet)
	{
		pRet->autorelease();
		//pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}	
}

void Swap::deal(int num)
{
	auto option = options.at(num).asValueVector();
	auto buyerVec = option.at(0).asValueVector();
	auto sellerVec = option.at(1).asValueVector();

	if(check(_buyer, buyerVec.at(0).asValueMap()) && check(_seller, sellerVec.at(0).asValueMap()))
	{
		realize(_buyer, buyerVec.at(0).asValueMap());
		realize(_buyer, buyerVec.at(1).asValueMap());
		int n1 = _buyer->getSwapCount();
		_buyer->setSwapCount(++n1);

		realize(_seller, sellerVec.at(0).asValueMap());
		realize(_seller, sellerVec.at(1).asValueMap());
		int n2 = _seller->getSwapCount();
		_seller->setSwapCount(++n2);

		conditions[_buyer->getName()+"_swap_ok"] = LuaValue::intValue(1);
		
		if(!_seller->getModelByName("dealAppear").isNull())
		{
			_seller->disappear();
		}

		PopPanel::getInstance()->removePanel(_swapPanel);
		this->removeFromParent();
	}
	else
	{

	}
}

bool Swap::check(Actor* actor, ValueMap map)
{
	auto player = dynamic_cast<Player*>(actor);
	for(auto pair:map)
	{
		auto type = pair.first;
		auto num = pair.second.asInt();
		if(type == "gold")
		{
			if(actor->getGold() + num < 0)
			{
				PopPanel::getInstance()->note("cantSwap","type:Text---text:"+ a2u("金币不足"));
				return false;
			}
		}
		else if(type == "hp")
		{
			if(actor->getHp() + num < 0)
				return false;
		}
		else if(type == "str")
		{
			if(actor->getStr() + num < 0)
				return false;
		}
		else if(type == "def")
		{
			if(actor->getDef() + num < 0)
				return false;
		}
		else if(type == "xp")
		{
			if(actor->getXp() + num < 0)
				return false;
		}
		else if(type == "level")
		{
			if(actor->getLevel() + num  < 0)
				return false;
		}
		else if(type == "key1")
		{
			if(player->getItemBag()->getPropSizeByType(2002001) + num < 0)
			{
				PopPanel::getInstance()->note("cantSwap","type:Text---text:"+ a2u("蓝钥匙不足"));
				return false;
			}
		}
		else if(type == "key2")
		{
			if(player->getItemBag()->getPropSizeByType(2002002) + num < 0)
			{
				PopPanel::getInstance()->note("cantSwap","type:Text---text:"+ a2u("黄钥匙不足"));
				return false;
			}
		}
		else if(type == "key3")
		{
			if(player->getItemBag()->getPropSizeByType(2002003) + num < 0)
			{
				PopPanel::getInstance()->note("cantSwap","type:Text---text:"+ a2u("红钥匙不足"));
				return false;
			}
		}
		else if(type == "sparPatch")
		{
			if(player->getItemBag()->getPropSizeByType(2002005) + num < 0)
			{
				PopPanel::getInstance()->note("cantSwap","type:Text---text:"+ a2u("晶石碎片不足"));
				return false;
			}
		}
	}
	return true;
}

void Swap::realize(Actor* actor, ValueMap map)
{
	auto player = dynamic_cast<Player*>(actor);
	for(auto pair:map)
	{
		auto type = pair.first;
		auto num = pair.second.asInt();
		if(type == "gold")
		{
			actor->addGold(num);
		}
		else if(type == "hp")
		{
			actor->addHp(num);
		}
		else if(type == "str")
		{
			actor->addStr(num);
		}
		else if(type == "def")
		{
			actor->addDef(num);
		}
		else if(type == "xp")
		{
			actor->addXp(num);
		}
		else if(type == "level")
		{
			actor->addLevel(num);
		}
		else if(type == "key1")
		{
			if(num >= 0)
			{
				player->addPropByType(2002001,num);
			}
			else
			{
				player->usePropByType(2002001,-num);
			}
		}
		else if(type == "key2")
		{
			if(num >= 0)
			{
				player->addPropByType(2002002,num);
			}
			else
			{
				player->usePropByType(2002002,-num);
			}
		}
		else if(type == "key3")
		{
			if(num >= 0)
			{
				player->addPropByType(2002003,num);
			}
			else
			{
				player->usePropByType(2002003,-num);
			}
		}
		else if(type == "sparPatch")
		{
			if(num >= 0)
			{
				player->addPropByType(2002005,num);
			}
			else
			{
				player->usePropByType(2002005,-num);
			}
		}
		//KB,PJ,BJ,XX,LJ,RD,RH,GD,FT,SB
		else if(type == "KBspar")
		{
			player->addSparByType(2006001);
		}
		else if(type == "PJspar")
		{
			player->addSparByType(2006002);
		}
		else if(type == "BJspar")
		{
			player->addSparByType(2006003);
		}
		else if(type == "XXspar")
		{
			player->addSparByType(2006004);
		}
		else if(type == "LJspar")
		{
			player->addSparByType(2006005);
		}
		else if(type == "RDspar")
		{
			player->addSparByType(2006006);
		}
		else if(type == "RHspar")
		{
			player->addSparByType(2006007);
		}
		else if(type == "GDspar")
		{
			player->addSparByType(2006008);
		}
		else if(type == "FTspar")
		{
			player->addSparByType(2006009);
		}
		else if(type == "SBspar")
		{
			player->addSparByType(2006010);
		}
	}
}