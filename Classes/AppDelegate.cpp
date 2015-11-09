#include "AppDelegate.h"
#include "scene/StartScene.h"
#include "scene/GameScene.h"
#include "scene/LogoScene.h"
#include "scene/PreScene.h"
#include "utils/AnimatePacker.h"
#include "utils/SendLuaData.h"
#include "utils/PlotScript.h"
#include "CCLuaEngine.h"
#include "SimpleAudioEngine.h"
#include "Detect.h"
using namespace CocosDenshion;

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {

	Configuration::getInstance()->loadConfigFile("configs/config-tower.plist");
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }

	Texture2D::PVRImagesHavePremultipliedAlpha(true);

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	
#else
	glview->setFrameSize(480, 800);
#endif
	
	auto designSize = Size(480, 800);

	//glview->setDesignResolutionSize(800, 480, ResolutionPolicy::NO_BORDER);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	// a bug in DirectX 11 level9-x on the device prevents ResolutionPolicy::NO_BORDER from working correctly
	glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::SHOW_ALL);
#else
	glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::NO_BORDER);
#endif

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

	preHandle();

	Detect::shareDetect();

	// create a scene. it's an autorelease object
	auto scene = LogoScene::create();

    // run
    director->runWithScene(scene);
	
	
	/*log("Str2 %s",PlotScript::sharedHD()->getLuaVarString("script/Test.lua","luaStr2"));
	log("age = %s",PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "luaTable","age"));
	log("name = %s",PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "luaTable","name"));
	log("sex = %s",PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "luaTable","sex"));
	log("Table = %s",PlotScript::sharedHD()->getLuaVarTable("script/Test.lua", "luaTable"));
	log("Call Lua Function Back: %s",PlotScript::sharedHD()->callLuaFunction("script/Test.lua", "luaLogString"));*/

    return true;
}

void AppDelegate::preHandle()
{
	Director::sharedDirector()->setProjection(kCCDirectorProjection2D);

	// ¼ÓÔØÍ¼Æ¬¶¯»­×ÊÔ´
	/*auto fullPath = FileUtils::getInstance()->fullPathForFilename("floor/floor1.tmx");
	std::string dir;
	if (fullPath.find_last_of("/") != std::string::npos)
	{
	dir = fullPath.substr(0, fullPath.find_last_of("/") + 1);
	}*/

	std::string plistPath;
	std::string pngPath;
	auto spriteFrameCache = SpriteFrameCache::getInstance();
	auto fileUtils = FileUtils::getInstance();
	auto animationPacker = AnimatePacker::getInstance();

	/*plistPath = fileUtils->fullPathForFilename("image/object/actor.plist");
	pngPath = fileUtils->fullPathForFilename("image/object/actor.pvr.ccz");
	spriteFrameCache->addSpriteFramesWithFile(plistPath, pngPath);

	plistPath = fileUtils->fullPathForFilename("image/object/item.plist");
	pngPath = fileUtils->fullPathForFilename("image/object/item.pvr.ccz");
	spriteFrameCache->addSpriteFramesWithFile(plistPath, pngPath);

	plistPath = fileUtils->fullPathForFilename("image/object/effect.plist");
	pngPath = fileUtils->fullPathForFilename("image/object/effect.pvr.ccz");
	spriteFrameCache->addSpriteFramesWithFile(plistPath, pngPath);*/

	plistPath = fileUtils->fullPathForFilename("image/object/cell.plist");
	pngPath = fileUtils->fullPathForFilename("image/object/cell.pvr.ccz");
	spriteFrameCache->addSpriteFramesWithFile(plistPath, pngPath);

	plistPath = fileUtils->fullPathForFilename("image/tileSets/head.plist");
	pngPath = fileUtils->fullPathForFilename("image/tileSets/head.pvr.ccz");
	spriteFrameCache->addSpriteFramesWithFile(plistPath, pngPath);

	/*plistPath = fileUtils->fullPathForFilename("image/tileSets/other.plist");
	pngPath = fileUtils->fullPathForFilename("image/tileSets/other.pvr.ccz");
	spriteFrameCache->addSpriteFramesWithFile(plistPath, pngPath);*/

	/*animationPacker->loadAnimations("image/object/item_ani.xml");
	animationPacker->loadAnimations("image/object/actor_ani.xml");
	animationPacker->loadAnimations("image/object/effect_ani.xml");*/

	animationPacker->loadAnimations("image/object/cell_ani.xml");

	textDic = Dictionary::createWithContentsOfFile("text.plist");
	textDic->retain();

	// ±³¾°ÒôÀÖ
	ValueVector bgMics;
	stringToValueVector(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", "bgMusics"),"|",bgMics);
	for(auto i:bgMics)
	{
		auto name = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "musics",i.asString().c_str());
		// android effect only support ogg
		std::string newName;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	newName = StringUtils::format("%s.ogg",name);
#else
	newName = StringUtils::format("%s.mp3",name);
#endif
		SimpleAudioEngine::getInstance()->preloadBackgroundMusic(newName.c_str());
	}
	// ÒôÐ§
	ValueVector effectMics;
	stringToValueVector(PlotScript::sharedHD()->getLuaVarString("script/Test.lua", "effectMusics"),"|",effectMics);
	for(auto i:effectMics)
	{
		auto name = PlotScript::sharedHD()->getLuaVarOneOfTable("script/Test.lua", "musics",i.asString().c_str());
		std::string newName;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		newName = StringUtils::format("%s.ogg",name);
#else
		newName = StringUtils::format("%s.mp3",name);
#endif
		SimpleAudioEngine::getInstance()->preloadEffect(newName.c_str());
	}
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
