#include "StartRainCloudScene.h"
#include "RainCloudScene.h"

#include "GameDefinition.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GameCenterBridge.h"
#include "AdmobHelperBridge.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

USING_NS_CC;

Scene* StartRainCloud::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = StartRainCloud::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool StartRainCloud::init()
{
	if (!Layer::init()) {
		return false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (AdmobHelperBridge::shared()->getIsBannered()) {
        AdmobHelperBridge::shared()->hideAD();
    }
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	jobject activityObj;
	
	bool isHave = JniHelper::getStaticMethodInfo(t, 
						"org/cocos2dx/cpp/AppActivity", 
						"HideAdPopup", 
						"()V");
						
	if (isHave) {
		activityObj = t.env->CallStaticObjectMethod(t.classID, t.methodID);
	}
#endif 
    
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(SOUND_CLICK);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0.4);
    

	winSize = Director::getInstance()->getVisibleSize();
	random = Random::createRandom();
    checkSaveFile();

	auto startItem = MenuItemImage::create(
		"labelTrans.png",
		"labelTrans.png",
		CC_CALLBACK_1(StartRainCloud::menuStartCallback, this));
	startItem->setPosition(winSize.width / 2, winSize.height*0.45);
    
    auto gamecenterItem = MenuItemImage::create(
        "labelTrans.png",
        "labelTrans.png",
        CC_CALLBACK_1(StartRainCloud::menuGameCenterCallback, this));
    gamecenterItem->setPosition(winSize.width / 2, winSize.height*0.3);

	char buf[256];
	// create menu, it's an autorelease object
	auto menu1 = Menu::create(startItem, NULL);
	menu1->setPosition(Vec2::ZERO);
	menu1->setAnchorPoint(Point(0.5, 0.5));
	this->addChild(menu1, Z_BUTTON);
	sprintf(buf, "Start");
	auto label1 = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	label1->setPosition(winSize.width / 2, winSize.height*0.45);
	label1->setColor(Color3B(124, 234, 183));
	this->addChild(label1, Z_BUTTON + 1);
    
    auto menu3 = Menu::create(gamecenterItem, NULL);
	menu3->setPosition(Vec2::ZERO);
	menu3->setAnchorPoint(Point(0.5,0.5));
	this->addChild(menu3, Z_BUTTON);
    sprintf(buf, "Scores");
    auto label3 = LabelTTF::create(buf,"fonts/Chalkduster.ttf", 50);
    label3->setPosition(winSize.width/2, winSize.height*0.3);
    label3->setColor(Color3B(241,209,159));
    this->addChild(label3, Z_BUTTON+1);    

	auto bg1 = Sprite::create("background.png");
	bg1->setPosition(Point(winSize.width / 2, winSize.height / 2));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	float verticalRatio = winSize.height / 640.0;
	float horizaonRatio = winSize.width / 1136.0;
	float scaleFactor = MAX(verticalRatio, horizaonRatio);
	bg1->setScale(scaleFactor);
#endif
	this->addChild(bg1, Z_BACKGROUND);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	sprintf(buf, "%s", "Rain & Clouds");
#else
	sprintf(buf, "%s", "Rain && Clouds");
#endif
	auto titleLabel = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 80);
	titleLabel->setPosition(winSize.width / 2, winSize.height*0.8);
	titleLabel->setColor(Color3B(255, 255, 255));
	this->addChild(titleLabel, Z_BUTTON + 1);

	schedule(schedule_selector(StartRainCloud::cloudAnimation), 1.5);
	schedule(schedule_selector(StartRainCloud::rainEmitter), 1.0);

	return true;
}

void StartRainCloud::rainEmitter(float dt)
{
	auto emitter = ParticleRain::createWithTotalParticles(300);
	emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("Raindrop.png"));
	emitter->setPosition(Point(winSize.width / 2, winSize.height*1.0));
	emitter->setScale(4);
	emitter->setLife(3);
	emitter->setDuration(3);
	emitter->setEmissionRate(70);
	this->addChild(emitter, Z_EFFECT);
}

void StartRainCloud::cloudAnimation(float dt)
{
    float step = winSize.height / 8;
    float randomHeight = step * random->getUniformRandomAB(0, 8+1);
	Point p = Point(winSize.width+200, randomHeight);
	auto cloud = makeCloud(p);
	cloud->setPosition(p);
	cloud->setTag(100);
	this->addChild(cloud, Z_GAME);
	float timeShowing = random->getNormalRandom(10, 2);
	auto move = MoveBy::create(timeShowing, Point(-(winSize.width + winSize.width), 0));
	auto removeSprite = CallFunc::create(CC_CALLBACK_0(StartRainCloud::removeCloud, this, cloud));
	auto action = Sequence::create(move, removeSprite, NULL);
	cloud->runAction(action);
}


Sprite* StartRainCloud::makeCloud(Point p)
{
	Sprite* cloud;
	int lotto = random->getUniformRandomAB(0, 3);
	switch (lotto) {
	case 0:
		cloud = Sprite::create("clouds-1.png");
		break;
	case 1:
		cloud = Sprite::create("clouds-2.png");
		break;
	case 2:
		cloud = Sprite::create("clouds-3.png");
		break;
	case 3:
		cloud = Sprite::create("clouds-4.png");
		break;
	default:
		cloud = Sprite::create("clouds-1.png");
		break;
	}

	return cloud;
}

void StartRainCloud::checkSaveFile()
{
	std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + SAVE_FILE;
    
	bool status = FileUtils::getInstance()->isFileExist(fullPath.c_str());
	if (!status) {
		saveSettingData();
	}
}

void StartRainCloud::saveSettingData()
{
    bestDistanceScore = 0.0f;
	curreuntDistanceScore = 0.0f;
	isBest = 0;

    auto rootDic = Dictionary::create();
    auto bestDistanceScoreFloat = Float::create(bestDistanceScore);
    auto currentDistanceScoreFloat = Float::create(curreuntDistanceScore);
    auto isBestInt = Integer::create(isBest);
    
    rootDic->setObject(bestDistanceScoreFloat, "key_bestDistance");
	rootDic->setObject(currentDistanceScoreFloat, "key_currentDistance");
	rootDic->setObject(isBestInt, "key_isBest");
    
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
	std::string fullPath = writablePath + SAVE_FILE;
	rootDic->writeToFile(fullPath.c_str());
}

void StartRainCloud::removeCloud(Sprite* cloud)
{
	this->removeChild(cloud, true);
}

void StartRainCloud::menuStartCallback(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_CLICK);
	Scene* game = RainClouds::createScene();
	Director::getInstance()->replaceScene(game);
}

void StartRainCloud::menuGameCenterCallback(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_CLICK);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    GameCenterBridge::shared()->showGameCenter();
#endif
}
