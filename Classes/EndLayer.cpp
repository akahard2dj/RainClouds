#include "EndLayer.h"
#include "GameDefinition.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GameCenterBridge.h"
#include "AdmobHelperBridge.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

USING_NS_CC;

bool EndLayer::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0)))	{
		return false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	AdmobHelperBridge::shared()->showAD();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	jobject activityObj;
	
	bool isHave = JniHelper::getStaticMethodInfo(t, 
						"org/cocos2dx/cpp/AppActivity", 
						"ShowAdPopup", 
						"()V");
						
	if (isHave) {
		activityObj = t.env->CallStaticObjectMethod(t.classID, t.methodID);
	}
#endif
    
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_END);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_CLICK);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0.3);
	
	auto index = __Integer::create(0);
	NotificationCenter::getInstance()->postNotification("NotificationEnd", index);
    loadSettingData();

	winSize = Director::getInstance()->getWinSize();

	char buf[256];
	LabelTTF* textMissionOver;

	// Title ment
	if (isBest) {
		sprintf(buf, "%s", "Pioneer!");
        sprintf(stampName, "%s", "WowStamp.png");
	}
	else {
		if (currentDistanceScore > 0.5 * bestDistanceScore) {
			sprintf(buf, "%s", "Explorer!");
            sprintf(stampName, "%s", "OKStamp.png");
		}
		else {
			sprintf(buf, "%s", "Novice Traveler!");
            sprintf(stampName, "%s", "ClapStamp.png");
		}
	}
	textMissionOver = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	//auto text = Sprite::create("gameover.png");
	textMissionOver->setPosition(Point(winSize.width / 2, winSize.height*0.85));
	this->addChild(textMissionOver, Z_GAME);

	// Current Score
	sprintf(buf, "%s", "Travel Mileage - ");

	auto textScore = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	textScore->setAnchorPoint(Point(0.0, 0.5));
	textScore->setPosition(Point(winSize.width*0.05, winSize.height*0.68));
	textScore->setColor(Color3B(255, 255, 0));
	this->addChild(textScore, Z_GAME);

	sprintf(buf, "%10d meter", (int)currentDistanceScore);
	auto textScoreDist = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	textScoreDist->setAnchorPoint(Point(0.0, 0.5));
	textScoreDist->setPosition(Point(winSize.width*0.5, winSize.height*0.68));
	textScoreDist->setColor(Color3B(255, 255, 0));
	this->addChild(textScoreDist, Z_GAME);

	// Best Score
	sprintf(buf, "%s", "Best Mileage - ");

	auto textBestScore = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	textBestScore->setAnchorPoint(Point(0.0, 0.5));
	textBestScore->setPosition(Point(winSize.width*0.05, winSize.height*0.56));
	textBestScore->setColor(Color3B(255, 255, 255));
	this->addChild(textBestScore, Z_GAME);

	sprintf(buf, "%10d meter", (int)bestDistanceScore);
	auto textBestScoreDist = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	textBestScoreDist->setAnchorPoint(Point(0.0, 0.5));
	textBestScoreDist->setPosition(Point(winSize.width*0.5, winSize.height*0.56));
	textBestScoreDist->setColor(Color3B(255, 255, 255));
	this->addChild(textBestScoreDist, Z_GAME);

	// MenuItem

	auto menuItem2 = MenuItemImage::create("labelTrans.png",
		"labelTrans.png",
		CC_CALLBACK_1(EndLayer::doCloseLayer, this));
	auto menuItem3 = MenuItemImage::create("labelTrans.png",
		"labelTrans.png",
		CC_CALLBACK_1(EndLayer::doCloseLayer, this));
	
	// Menu Item2 : Restart
	//menuItem2->setPosition(Point(winSize.width / 2, winSize.height*0.35));
	menuItem2->setTag(2);

	sprintf(buf, "%s", "One More Time !");
	auto textMenuLabel2 = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 40);
	textMenuLabel2->setPosition(Point(winSize.width / 2, winSize.height*0.35));
	textMenuLabel2->setColor(Color3B(43, 193, 235));
	this->addChild(textMenuLabel2, Z_GAME + 1);

	// Menu Item3 : Exit
	//menuItem3->setPosition(Point(winSize.width / 2, winSize.height*0.24));
	menuItem3->setTag(3);

	sprintf(buf, "%s", "Go Home");
	auto textMenuLabel3 = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 40);
	textMenuLabel3->setPosition(Point(winSize.width / 2, winSize.height*0.24));
	textMenuLabel3->setColor(Color3B(195, 50, 242));
	this->addChild(textMenuLabel3, Z_GAME + 1);

	auto pMenu = Menu::create(menuItem2, menuItem3, NULL);
	//pMenu->alignItemsVertically();
	pMenu->setPosition(Vec2::ZERO);
	menuItem2->setPosition(winSize.width/2, winSize.height*0.35);
	menuItem3->setPosition(winSize.width/2, winSize.height*0.24);
	this->addChild(pMenu);

	auto blackboard = Sprite::create("blackboard.png");
	blackboard->setPosition(Point(winSize.width / 2, winSize.height / 2));
	this->addChild(blackboard, Z_GAME - 1);
	backLayer = LayerColor::create(Color4B(0, 0, 0, 200), winSize.width, winSize.height);
	backLayer->setAnchorPoint(Point(0, 0));
	backLayer->setPosition(Point(0, 0));
	this->addChild(backLayer, Z_GAME - 2);

	if (isBest == 1) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		GameCenterBridge::shared()->reportScore((long long)bestDistanceScore);
        log("report score");
#endif
	}
    
    scheduleOnce(schedule_selector(EndLayer::doStamp), 1);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(EndLayer::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(EndLayer::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}

void EndLayer::doStamp(float dt) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_END);
    //auto stamp = Sprite::create("stamp.png");
    auto stamp = Sprite::create((std::string)stampName);
    stamp->setPosition(Point(winSize.width*0.85,winSize.height*0.85));
    stamp->setScale(0.05, 0.05);
    stamp->setRotation(15);
    stamp->runAction(FadeIn::create(0.2f));
    this->addChild(stamp, 10);
    
    auto m1 = MoveBy::create(0.05f, Vec2(0, winSize.height * 0.01));
    auto m2 = MoveBy::create(0.05f, Vec2(0, -winSize.height * 0.01));
    auto shake = Sequence::create(m1, m2, NULL);
    auto scaleUp = ScaleTo::create(0.05, 0.6);
    stamp->runAction(Sequence::create(scaleUp, NULL));
    
    auto shakeAnim = Repeat::create(shake, 5);
    this->runAction(shakeAnim);
}

void EndLayer::doCloseLayer(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_CLICK);
	MenuItem* pMenuItem = (MenuItem*)pSender;
	int tag = pMenuItem->getTag();

	__Integer* tagNumber = NULL;
	tagNumber = __Integer::create(tag);
	NotificationCenter::getInstance()->postNotification("NotificationPopup", tagNumber);
	this->removeFromParentAndCleanup(true);
}

void EndLayer::loadSettingData()
{
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + SAVE_FILE;
    
	auto loadDict = __Dictionary::createWithContentsOfFile(fullPath.c_str());
	auto floatValue1 = (__String*)loadDict->objectForKey("key_bestDistance");
	auto floatValue2 = (__String*)loadDict->objectForKey("key_currentDistance");
	auto intValue3 = (__String*)loadDict->objectForKey("key_isBest");
    
	if (floatValue1 != NULL) {
		bestDistanceScore = std::atof(floatValue1->getCString());
	} else {
		bestDistanceScore = 0.0f;
	}
    
	if (floatValue2 != NULL) {
		currentDistanceScore = std::atof(floatValue2->getCString());
	} else {
		currentDistanceScore = 0.0f;
	}
    
	if (intValue3 != NULL) {
		isBest = std::atoi(intValue3->getCString());
	} else {
		isBest = 0;
	}
    log("End Layer: %f %f %d",bestDistanceScore,currentDistanceScore,isBest);
}

bool EndLayer::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}
void EndLayer::onTouchEnded(Touch* touch, Event* event)
{
	//nothing..
}
