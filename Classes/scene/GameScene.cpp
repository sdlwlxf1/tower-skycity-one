#include "GameScene.h"
#include "cell/actor/Actor.h"
#include "Detect.h"
#include "UI/HudPanel.h"
#include "utils/PlotScript.h"
#include "cell/CellFactory.h"
#include "cell/actor/Actor.h"
#include "mutual/ChatManager.h"
#include "utils/SpotLight.h"
#include "utils/Altar.h"
#include "ui/ShareAwardPanel.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

bool GameScene::init()
{
	bool bRet = false;
	do{
		setName("gameScene");

		//_listener = EventListenerCustom::create(this->getName(), CC_CALLBACK_1(GameScene::onTrigger, this));
		_listener = EventListenerCustom::create(getName(), [=](EventCustom *event){onTrigger(event);});
		_eventDispatcher->addEventListenerWithFixedPriority(_listener, -1);
		CC_SAFE_RETAIN(_listener);

		auto detect = Detect::shareDetect();

		_hud = HudPanel::create("hud");
		_world = World::create();
		_world->setPosition(20, 120);
		detect->setWorld(_world);

		addChild(_world);
		addChild(_hud);
		addChild(Detect::shareDetect());

		PlotScript::sharedHD()->registerCppFunction("script/Test.lua");
		
		_color = LayerColor::create(Color4B::BLACK);
		addChild(_color);

		Detect::shareDetect()->getUser()->setFirstOpen(false);
		//getHud()->updateBuffList();

		bRet = true;
	} while (0);
	return bRet;
}

void GameScene::onTouchDown(Ref *pSender, Widget::TouchEventType type)
{
	
}

bool GameScene::onTrigger(EventCustom *event)
{
	return false;
}

void GameScene::onEnterTransitionDidFinish()
{
	Scene::onEnterTransitionDidFinish();
	auto detect = Detect::shareDetect();
	//detect->setCurFloor(1, 1);

	//ValueMap map;
	//map["typeId"] = 1001001;
	//map["name"] = "player";
	//auto player = Player::create(map);

	////Detect::shareDetect()->addCellToWaitingRoom(player);
	////detect->setPlayer(player);
	//detect->addCell(player,Point(5,0),1,1);
	//detect->setPlayer(player);

	PlotScript::sharedHD()->callLuaFunction("script/Test.lua", "initCondition");
	PlotScript::sharedHD()->callLuaFunction("script/Test.lua", "init");

	_color->runAction(Sequence::create(FadeOut::create(1.5f),CallFunc::create([=](){removeChild(_color);}),nullptr));

	if(Detect::shareDetect()->load == true)
	{
		auto loadMap = FileUtils::getInstance()->getValueMapFromFile(detect->loadPath);
		Detect::shareDetect()->loadModel(loadMap);
	}
	
	auto ss = PlotScript::sharedHD()->getLuaVarString("script/Test.lua", "initDelay");
	schedule(schedule_selector(GameScene::plotUpdate), 0.05f);
	
	// 判断是否已经开启分享奖励
	if(Detect::shareDetect()->getUser()->getShareRewardOpen() == true)
	{
		// 分享奖励界面
		runAction(Sequence::create(DelayTime::create(2.0f),CallFunc::create([](){PopPanel::getInstance()->addPanel(ShareAwardPanel::create("shareAward"),1);}),nullptr));
	}
	/*Size winSize = Director::getInstance()->getWinSize();

	RenderTexture * renderLayer = RenderTexture::create(winSize.width, winSize.height, kCCTexture2DPixelFormat_RGBA8888);
	renderLayer->setPosition(ccp(winSize.width / 2, winSize.height / 2));
	BlendFunc blend = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
	renderLayer->getSprite()->setBlendFunc(blend);
	addChild(renderLayer);

	SpotLight * spotLight = SpotLight::create(renderLayer, 100, Color4F(0 , 0, 0, 180));
	addChild(spotLight, 1000);
	spotLight->setPosition(Vec2(200,200));
	spotLight->setTag(2012);*/
}

void GameScene::onEnter()
{
	Scene::onEnter();
}

void GameScene::plotUpdate(float dt)
{
	PlotScript::sharedHD()->callLuaFunction("script/Test.lua", "plotUpdate");
}

void GameScene::onExit()
{
	Scene::onExit();
}

GameScene::~GameScene()
{
	_eventDispatcher->removeEventListener(_listener);
}

void GameScene::resetInstance()
{
	PlotScript::sharedHD()->callLuaFunction("script/Test.lua", "endGame");
	unschedule(schedule_selector(GameScene::plotUpdate));
	PopPanel::getInstance()->reset();
	ChatManager::getInstance()->reset();
	Detect::shareDetect()->reset();
}

void GameScene::onHudTrigger(EventCustom *event)
{
	
}
