//
// RainCloudScene.cpp
// RainCloud
//
// Created by Dowon Yi on 2014. 8. 14..
//
//

#include "RainCloudScene.h"
#include "MyBodyParser.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GameCenterBridge.h"
#include "AdmobHelperBridge.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

USING_NS_CC;
using namespace cocostudio;

Scene* RainClouds::createScene()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vect gravity = Vect((float)GRAVITY_X, -(float)GRAVITY_Y);

	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(gravity);

	auto body = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3.0f);
	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	edgeNode->setPhysicsBody(body);

	auto layer = RainClouds::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	scene->addChild(layer);

	return scene;
}

bool RainClouds::init()
{
	//////////////////////////////
	// 1. super init first
	if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255)))
	{
		return false;
	}
	winSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
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
    
	auto closeItem = MenuItemImage::create(
		"pause.png",
		"pause.png",
		CC_CALLBACK_1(RainClouds::optionOpenCallback, this));
	closeItem->setPosition(Vec2(origin.x + winSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));
	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, Z_BUTTON - 1);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(RainClouds::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(RainClouds::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(RainClouds::onContactBegin, this);
	contactListener->onContactSeperate = CC_CALLBACK_1(RainClouds::onContactSeperate, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	NotificationCenter::getInstance()->addObserver(
		this, 
		callfuncO_selector(RainClouds::doNotification), 
		"NotificationPopup", 
		NULL);

	initGame();
	initAnimation();
    initSound();

	return true;
}

void RainClouds::initGame()
{
	isUFODataLoaded = false;
	isTouched = false;
	isStarted = false;
	isUpperTouched = false;
    isLowerTouched = false;
	isFull = false;
	isEnded = false;
    isBest = 0;

	HP = 100.0;
	MP = 0.0;
	nRowGrid = START_GRID;
	totalDistance = 0.0f;

	showingCloudInterval = 1.2;
    
    loadSettingData();

	unsigned long init[4] = { (unsigned)time(NULL), (unsigned)time(NULL), (unsigned)time(NULL), (unsigned)time(NULL) };
	random = Random::createRandom(init, 4);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    long long gamecenterBestDistance = 0;
    
    gamecenterBestDistance = GameCenterBridge::shared()->getScore();
    log("BestScore: %lld , %lld", (long long)bestDistance, gamecenterBestDistance);
    
    if ((long long)bestDistance > gamecenterBestDistance) {
        GameCenterBridge::shared()->reportScore((long long)bestDistance);
    } else {
        bestDistance = (long long)gamecenterBestDistance;
    }
#endif

	char buf[256];
	sprintf(buf, "%d%s", (int)totalDistance, " meter");
	labelForDistance = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 40, Size(winSize.width * 0.5, winSize.height * 0.1), TextHAlignment::RIGHT, TextVAlignment::BOTTOM);
	labelForDistance->setPosition(winSize.width*0.7, winSize.height*0.9);
	this->addChild(labelForDistance, Z_EFFECT + 1);

	auto bg = Sprite::create("background.png");
	bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	float verticalRatio = winSize.height / 640.0;
	float horizaonRatio = winSize.width / 1136.0;
	float scaleFactor = MAX(verticalRatio, horizaonRatio);
	bg->setScale(scaleFactor);
#endif
	this->addChild(bg, Z_BACKGROUND);

	status = Sprite::create("status.png");
	status->setAnchorPoint(Point(0.5, 0));
	status->setPosition(Point(winSize.width*0.95, winSize.height*0.3));
	this->addChild(status, Z_EFFECT + 1);
	
	auto statusBorder = Sprite::create("status-border.png");
	statusBorder->setAnchorPoint(Vec2(0.5, 0.0));
	statusBorder->setPosition(winSize.width*0.95, winSize.height*0.3);
	this->addChild(statusBorder, Z_EFFECT - 1);

	eventDispatcher = _eventDispatcher;
	starTouch = Sprite::create("starIdentifier.png");
	starTouch->setPosition(Point(winSize.width*0.05, winSize.height*0.9));
	this->addChild(starTouch, Z_EFFECT + 3);
	selectSpecialEventListener();

	statusStar = Sprite::create("status.png");
	statusStar->setPosition(Point(winSize.width*0.06, winSize.height*0.9));
	statusStar->setAnchorPoint(Point(0.5, 0));
	statusStar->setScaleY(0.0);
	statusStar->setRotation(90);
	this->addChild(statusStar, Z_EFFECT + 2);

	auto statusStarBorder = Sprite::create("status-border.png");
	statusStarBorder->setAnchorPoint(Vec2(0.5, 0.0));
	statusStarBorder->setPosition(winSize.width*0.06, winSize.height*0.9);
	statusStarBorder->setRotation(90);
	this->addChild(statusStarBorder, Z_EFFECT + 1);

	auto upper = makeBorder(Point(winSize.width / 2, winSize.height+BORDER_Y/2 - 40 ));
	upper->setTag(TAG_UPPER);
	upper->getPhysicsBody()->setDynamic(true);
	this->addChild(upper, Z_EFFECT);

	auto lower = makeBorder(Point(winSize.width / 2, 0-BORDER_Y/2));
	lower->setTag(TAG_LOWER);
	lower->getPhysicsBody()->setDynamic(true);
	this->addChild(lower, Z_EFFECT);
    
    touchStart = Sprite::create("touchStart.png");
    touchStart->setPosition(winSize.width*0.2, winSize.height/2);
    this->addChild(touchStart, Z_EFFECT);

	schedule(schedule_selector(RainClouds::ufoAnimation), 2.0*300.0/(float)GRAVITY_Y);
}

void RainClouds::initAnimation()
{
	ArmatureDataManager::getInstance()->addArmatureFileInfoAsync(
		"AnimationUFO.ExportJson", 
		this, 
		schedule_selector(RainClouds::onUFOAnimDataLoaded));
	ArmatureDataManager::getInstance()->addArmatureFileInfoAsync(
		"AnimationStar.ExportJson",
		this,
		schedule_selector(RainClouds::onStarAnimDataLoaded));
}

void RainClouds::initSound()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_CRASH);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_ITEM);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_HEAL);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_CLICK);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(SOUND_BGM);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0.4);
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.6);
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(SOUND_BGM, true);
}

void RainClouds::onStarAnimDataLoaded(float dt)
{
	starAnim = nullptr;
	starAnim = Armature::create("AnimationStar");
	starAnim->setPosition(winSize.width*0.05, winSize.height*0.9);
	starAnim->setScale(1.0/1.2);
	this->addChild(starAnim, Z_EFFECT + 3);
}

void RainClouds::onUFOAnimDataLoaded(float dt)
{
	UFOanim = nullptr;
	UFOanim = Armature::create("AnimationUFO");
	UFOanim->setPosition(winSize.width*0.2, winSize.height*0.9);
	UFOanim->setTag(TAG_UFO);
	//MyBodyParser::getInstance()->parseJsonFile("UFO.json");
	auto material = PhysicsMaterial(0.1f, 1.0f, 0.5f);
	//auto body = MyBodyParser::getInstance()->bodyFormJson(UFOanim, "UFO", material);
	auto body = PhysicsBody::createCircle(UFOanim->getContentSize().width / 6, material);
	if (body != nullptr) {
		isUFODataLoaded = true;
		UFOanim->setPhysicsBody(body);
	}
	else {
		isUFODataLoaded = false;
	}
	body->setDynamic(true);
	body->setCategoryBitmask(0x01); // 0000
	body->setContactTestBitmask(0x08); // 0100
	body->setCollisionBitmask(0x03); // 0011
	this->addChild(UFOanim, Z_EFFECT);
}

void RainClouds::addCloud(float dt)
{
	float step = winSize.height / nRowGrid;
	float randomHeight = step * random->getUniformRandomAB(0, (int)nRowGrid+1);
	float accel = ((int)totalDistance / 300) * 30;

	Point p = Point(winSize.width + 500, randomHeight);
	auto cloud = makeCloud(p);
	this->addChild(cloud, Z_GAME);

	float distanceWalking = winSize.width + 500;
	float cloudVelocity, timeShowing;
	cloudVelocity = 300 + random->getNormalRandom(0, 30) + accel;
	timeShowing = distanceWalking / cloudVelocity;
	
	auto move = MoveBy::create(timeShowing, Point(-(winSize.width + winSize.width), 0));
	auto removeSprite = CallFunc::create(CC_CALLBACK_0(RainClouds::removeItem, this, cloud));
	auto action = Sequence::create(move, removeSprite, NULL);
	cloud->runAction(action);
}

void RainClouds::addStar(float dt)
{
	Point p = Point(winSize.width + 500, random->getUniformRandom()*winSize.height);
	auto item = makeStar(p);
	item->setPosition(p);

	this->addChild(item, Z_GAME);
	float itemVelocity = 700 + random->getNormalRandom(0, 20);
	float distanceWalking = winSize.width + 500;
	float timeShowing = distanceWalking / itemVelocity;
	auto move = MoveBy::create(timeShowing, Point(-(winSize.width + winSize.width), 0));
	auto removeSprite = CallFunc::create(CC_CALLBACK_0(RainClouds::removeItem, this, item));
	auto action = Sequence::create(move, removeSprite, NULL);
	item->runAction(action);
}

void RainClouds::unscheduleAll()
{
    //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.1);
    
	starAnim->getAnimation()->stop();

	unschedule(schedule_selector(RainClouds::updateDistance));
	unschedule(schedule_selector(RainClouds::updateDistanceLabel));
	unschedule(schedule_selector(RainClouds::addCloud));
	unschedule(schedule_selector(RainClouds::addStar));

	unschedule(schedule_selector(RainClouds::updateStarTime));
	unschedule(schedule_selector(RainClouds::updateCloudTime));
}

void RainClouds::updateSchedule()
{
	schedule(schedule_selector(RainClouds::updateDistance));
	schedule(schedule_selector(RainClouds::updateDistanceLabel), 0.2);
	schedule(schedule_selector(RainClouds::addCloud), showingCloudInterval);
	schedule(schedule_selector(RainClouds::addStar), 2.0);

	schedule(schedule_selector(RainClouds::updateStarTime), 3);
	schedule(schedule_selector(RainClouds::updateCloudTime), 8);
}

void RainClouds::updateStarTime(float dt)
{
	unschedule(schedule_selector(RainClouds::addStar));

	int lotto = random->getUniformRandomAB(0, 4);
	if (lotto == 4) {
		schedule(schedule_selector(RainClouds::addStar), 0.4);
	}
	else {
		schedule(schedule_selector(RainClouds::addStar), 2.0);
	}
}

void RainClouds::updateCloudTime(float dt)
{
	unschedule(schedule_selector(RainClouds::addCloud));

	showingCloudInterval = showingCloudInterval - 0.03;
	if (showingCloudInterval < 0.3) {
		showingCloudInterval = 0.3;
	}

	nRowGrid = nRowGrid + 0.3;
	if (nRowGrid > MAX_GRID) {
		nRowGrid = MAX_GRID;
	}
	log("N GRID, %f   ShowingTime, %f", nRowGrid, showingCloudInterval);
	schedule(schedule_selector(RainClouds::addCloud), showingCloudInterval);
}

void RainClouds::updateDistance(float dt)
{
	float move = 0.1 + totalDistance * 0.0001;
	totalDistance = totalDistance + move;
    
    if (totalDistance > bestDistance) {
        isBest = 1;
        bestDistance = totalDistance;
    }
	
}

void RainClouds::updateDistanceLabel(float dt)
{
	char buf[100];
	sprintf(buf, "%d meter", (int)totalDistance);
	labelForDistance->setString(buf);
}

void RainClouds::updateMP(float amount)
{
	MP = MP + 1;
	if (MP >= MP_MAX) {
		MP = MP_MAX;
		starAnim->getAnimation()->playWithIndex(0);
		isFull = true;
	}

	auto action = Sequence::create(ScaleTo::create(0.5, 1.0, MP / MP_MAX), NULL);
	statusStar->runAction(action);
}

void RainClouds::updateHP(float amount)
{
	int flag1, flag2;
	float prevHP, currHP;
	prevHP = HP / HP_MAX;
	HP = HP - amount;
	currHP = HP / HP_MAX;

	if (prevHP > 0.7) {
		flag1 = 0;
	}
	else if (prevHP > 0.3 && prevHP <= 0.7) {
		flag1 = 1;
	}
	else if (prevHP <= 0.3) {
		flag1 = 2;
	}

	if (currHP > 0.7) {
		flag2 = 0;
	}
	else if (currHP > 0.3 && currHP <= 0.7) {
		flag2 = 1;
	}
	else if (currHP <= 0.3) {
		flag2 = 2;
	}

	if (flag1 != flag2) {
		if (currHP > 0.7) {
			status->setTexture("status.png");
		}
		else if (currHP > 0.3 && currHP <= 0.7) {
			status->setTexture("status-middle.png");
		}
		else if (currHP <= 0.3) {
			status->setTexture("status-end.png");
		}
	}

	if (HP < 0.0)
		HP = 0.0;

	if (HP == 0.0) {
        saveSettingData();
		isEnded = true;
		unscheduleAll();
		UFOanim->getAnimation()->stop();
		UFOanim->getAnimation()->playWithIndex(2);
        log("%d",isLowerTouched);
        if (isLowerTouched) {
            scheduleOnce(schedule_selector(RainClouds::showEndLayer), 0.5);
        } else {
            scheduleOnce(schedule_selector(RainClouds::showEndLayer), 1.3);
        }
	}

	auto action = Sequence::create(ScaleTo::create(0.5, 1.0, HP / HP_MAX), NULL);
	status->runAction(action);	
}

void RainClouds::showEndLayer(float dt) 
{
	auto pScene = EndLayer::create();
	pScene->setPosition(Vec2(0, -winSize.height));
	pScene->runAction(MoveTo::create(0.5, Vec2::ZERO));
	this->addChild(pScene, Z_BUTTON);
}

void RainClouds::showTouchStart(float dt)
{
    auto move = MoveBy::create(0.8, Vec2(0, -winSize.height*0.6));
    auto removeSprite = CallFunc::create(CC_CALLBACK_0(RainClouds::removeItem, this, touchStart));
    auto action = Sequence::create(EaseIn::create(move, 2), DelayTime::create(1.0), removeSprite, NULL);
    
    touchStart->runAction(action);
}

bool RainClouds::onContactBegin(PhysicsContact& contact)
{
	auto sp1 = (Sprite *)contact.getShapeA()->getBody()->getNode();
	auto sp2 = (Sprite *)contact.getShapeB()->getBody()->getNode();
	int tag1 = sp1->getTag();
	int tag2 = sp2->getTag();

	// Cloud
	if (tag1 == TAG_CLOUD || tag2 == TAG_CLOUD) {
		Sprite* pCloud;
		Sprite* pUFO;
		if (tag1 == TAG_CLOUD) {
			pCloud = sp1;
			pUFO = sp2;
		}
		if (tag2 == TAG_CLOUD) {
			pCloud = sp2;
			pUFO = sp1;
		}

		if (pCloud->getRotation() == 0.0) {
			pCloud->runAction(Sequence::create(FadeOut::create(0.3), NULL));
			rainEmitter();
			UFOanim->getAnimation()->stop();
			UFOanim->getAnimation()->playWithIndex(1);
            playSound(SOUND_CRASH);
			updateHP(CLOUD_CRASH);
		}
		pCloud->setRotation(0.01);
	}

	// Star
	if (tag1 == TAG_STAR || tag2 == TAG_STAR) {
		Sprite* pItem;
		Sprite* pUFO;
		if (tag1 == TAG_STAR) {
			pItem = sp1;
			pUFO = sp2;
		}
		if (tag2 == TAG_STAR) {
			pItem = sp2;
			pUFO = sp1;
		}
		
		if (pItem->isVisible()) {
			UFOanim->getAnimation()->stop();
			UFOanim->getAnimation()->playWithIndex(3);
            playSound(SOUND_ITEM);
			updateMP(1.0);
			pItem->setVisible(false);
			starAnimation(pUFO->getPosition());
		}
	}

	//Upper Border
	if (tag1 == TAG_UPPER || tag2 == TAG_UPPER) {
		Sprite* pBorder;
		Sprite* pUFO;
		if (tag1 == TAG_UPPER) {
			pBorder = sp1;
			pUFO = sp2;
		}
		if (tag2 == TAG_UPPER) {
			pBorder = sp2;
			pUFO = sp1;
		}
		isUpperTouched = true;
	}

	//Lower Border
	if (tag1 == TAG_LOWER || tag2 == TAG_LOWER) {
		Sprite* pBorder;
		Sprite* pUFO;
		if (tag1 == TAG_LOWER) {
			pBorder = sp1;
			pUFO = sp2;
		}
		if (tag2 == TAG_LOWER) {
			pBorder = sp2;
			pUFO = sp1;
		}
        isLowerTouched = true;
		updateHP(HP);
	}
	
	return true;
}

void RainClouds::onContactSeperate(PhysicsContact& contact)
{
	auto sp1 = (Sprite *)contact.getShapeA()->getBody()->getNode();
	auto sp2 = (Sprite *)contact.getShapeB()->getBody()->getNode();
	int tag1 = sp1->getTag();
	int tag2 = sp2->getTag();

	//Upper Border
	if (tag1 == 1 || tag2 == 1) {
		Sprite* pBorder;
		Sprite* pUFO;
		if (tag1 == 1) {
			pBorder = sp1;
			pUFO = sp2;
		}
		if (tag2 == 1) {
			pBorder = sp2;
			pUFO = sp1;
		}
		isUpperTouched = false;
	}
}

bool RainClouds::onTouchBegan(Touch* touch, Event* event)
{
	isTouched = true;
	if (isTouched && !isStarted) {
		unschedule(schedule_selector(RainClouds::ufoAnimation));
        scheduleOnce(schedule_selector(RainClouds::showTouchStart), 0.0);
		updateSchedule();
		isStarted = true;
	}

	return true;
}

void RainClouds::onTouchEnded(Touch* touch, Event* event)
{
	auto location = touch->getLocation();
	auto sprite = (Sprite *)getChildByTag(TAG_UFO);

	Vect velocity = Vect(0, 300);
	if (!isEnded) {
		if (!isUpperTouched) {
			sprite->getPhysicsBody()->setVelocity(velocity);
		}
	}
}

void RainClouds::selectSpecialEventListener()
{
	auto itemListener = EventListenerTouchOneByOne::create();
	itemListener->setSwallowTouches(true);
	itemListener->onTouchBegan = [=](Touch* touch, Event* event) {
		auto target = event->getCurrentTarget();
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width+30, s.height+30);
		if (rect.containsPoint(locationInNode)) {
			if (isFull) {
				HP = HP_MAX;
				MP = 0;

				status->setTexture("status.png");
				auto tAction1 = Sequence::create(ScaleTo::create(0.2, 1.0, MP / MP_MAX), NULL);
				statusStar->runAction(tAction1);
				auto tAction2 = Sequence::create(ScaleTo::create(0.2, 1.0, HP / HP_MAX), NULL);
				status->runAction(tAction2);
				
				UFOanim->getAnimation()->stop();
				UFOanim->getAnimation()->playWithIndex(0);
				starAnim->getAnimation()->stop();
                playSound(SOUND_HEAL);
				auto scaleAction = ScaleTo::create(0.3, 1.0/1.2);
				starAnim->runAction(scaleAction);
				isFull = false;
			}
			return true;
		}
		return false;
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(itemListener->clone(), starTouch);
}

void RainClouds::starAnimation(Point p)
{
	int starType = random->getUniformRandomAB(0, 3);
	Sprite* star;

	switch (starType) {
	case 0:
		star = Sprite::create("itemStar-1.png");
		break;
	case 1:
		star = Sprite::create("itemStar-2.png");
		break;
	case 2:
		star = Sprite::create("itemStar-3.png");
		break;
	case 3:
		star = Sprite::create("itemStar-4.png");
		break;
	default:
		star = Sprite::create("itemStar-1.png");
		break;
	}
	star->setPosition(p);
	auto moveAction = MoveTo::create(0.3, Point(winSize.width*0.06, winSize.height*0.9));
	auto scaleAction = ScaleTo::create(0.2, 0.5, 0.5);
	auto removeAction = CallFunc::create(CC_CALLBACK_0(RainClouds::removeItem, this, star));
	auto seqAction = Sequence::create(moveAction, removeAction, NULL);
	auto mixedAction = Spawn::create(seqAction, scaleAction, NULL);
	this->addChild(star, Z_EFFECT - 1);
	star->runAction(mixedAction);
}

void RainClouds::ufoAnimation(float dt)
{
	auto sprite = (Sprite *)getChildByTag(TAG_UFO);
	Vect velocity = Vect(0, 300);
	sprite->getPhysicsBody()->setVelocity(velocity);
}

Sprite* RainClouds::makeBorder(Point p)
{
	auto border = Sprite::create("border.png");
	auto material = PhysicsMaterial(0.1f, 1.0f, 0.5f);
	auto body = PhysicsBody::createBox(border->getContentSize(), material);
	border->setPhysicsBody(body);
	border->setPosition(p);
	body->setGravityEnable(false);
	body->setCategoryBitmask(0x08); // 1000
	body->setContactTestBitmask(0x01); // 0001
	body->setCollisionBitmask(0x07); // 0110

	return border;
}

Sprite* RainClouds::makeCloud(Point p)
{
	int cloudType = random->getUniformRandomAB(0, 3);
	Sprite* cloud;

	switch (cloudType) {
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
	cloud->setTag(TAG_CLOUD);

	MyBodyParser::getInstance()->parseJsonFile("clouds.json");
	auto material = PhysicsMaterial(0.0f, 1.0f, 0.5f);
	auto body = MyBodyParser::getInstance()->bodyFormJson(cloud, "clouds", material);
	if (body != nullptr) {
		cloud->setPhysicsBody(body);
		cloud->setPosition(p);
	}

	body->setGravityEnable(false);
	body->setCategoryBitmask(0x08); // 1000
	body->setContactTestBitmask(0x01); // 0001
	body->setCollisionBitmask(0x07); // 0110

	return cloud;
}

Sprite* RainClouds::makeStar(Point p)
{
	int starType = random->getUniformRandomAB(0, 3);
	Sprite* star;

	switch (starType) {
		case 0:
			star = Sprite::create("itemStar-1.png");
			break;
		case 1:
			star = Sprite::create("itemStar-2.png");
			break;
		case 2:
			star = Sprite::create("itemStar-3.png");
			break;
		case 3:
			star = Sprite::create("itemStar-4.png");
			break;
		default:
			star = Sprite::create("itemStar-1.png");
			break;
	}

	MyBodyParser::getInstance()->parseJsonFile("itemStar.json");
	auto material = PhysicsMaterial(0.1f, 1.0f, 0.5f);
	auto body = MyBodyParser::getInstance()->bodyFormJson(star, "itemStar", material);
	if (body != nullptr) {
		star->setPhysicsBody(body);
		star->setPosition(p);
	}
	star->setTag(TAG_STAR);
	body->setGravityEnable(false);
	body->setCategoryBitmask(0x08); // 1000
	body->setContactTestBitmask(0x01); // 0001
	body->setCollisionBitmask(0x07); // 0110

	return star;
}

void RainClouds::removeItem(Sprite* item)
{
	this->removeChild(item, true);
}

void RainClouds::rainEmitter()
{
	auto emitter = ParticleRain::createWithTotalParticles(300);
	emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("Raindrop.png"));
	emitter->setPosition(Point(winSize.width / 2, winSize.height*1.1));
	emitter->setScale(4);
	emitter->setLife(3);
	emitter->setDuration(3);
	emitter->setEmissionRate(70);
	this->addChild(emitter, Z_EFFECT);
}

void RainClouds::playSound(std::string fileName)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(fileName.c_str());
}

void RainClouds::tempRainEmitter(Point p)
{
	auto emitter = ParticleRain::createWithTotalParticles(300);
	emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("Raindrop.png"));
	emitter->setPosition(p);
	emitter->setScale(4);
	emitter->setLife(3);
	emitter->setDuration(3);
	emitter->setEmissionRate(70);
	this->addChild(emitter, Z_EFFECT);
}

void RainClouds::optionOpenCallback(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_CLICK);
	if (this->getChildByTag(TAG_POPUP) == NULL) {
		auto pScene = PopLayer::create();
		pScene->setTag(TAG_POPUP);
		this->addChild(pScene, Z_BUTTON);
	}
}

void RainClouds::doNotification(Ref* obj)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	jobject activityObj;
	
	bool isHave = JniHelper::getStaticMethodInfo(t, 
						"org/cocos2dx/cpp/AppActivity", 
						"HideAdPopup", 
						"()V");
#endif
	Scene* game;
	__Integer* tag = (__Integer*)obj;
	switch (tag->getValue()) {
        case 0: //Pause
            Director::getInstance()->pause();
            CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
            CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.1);
            break;

        case 1:	//Resume
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            if (AdmobHelperBridge::shared()->getIsBannered()) {
                AdmobHelperBridge::shared()->hideAD();
            }
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)	
			if (isHave) {
				activityObj = t.env->CallStaticObjectMethod(t.classID, t.methodID);
			}
#endif
            Director::getInstance()->resume();
            CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.6);
            break;

        case 2: //Restart
            NotificationCenter::getInstance()->removeAllObservers(this);
            Director::getInstance()->resume();
            game = RainClouds::createScene();
            Director::getInstance()->replaceScene(game);
            break;

        case 3: //Exit
            NotificationCenter::getInstance()->removeAllObservers(this);
            Director::getInstance()->resume();
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
            game = StartRainCloud::createScene();
            Director::getInstance()->replaceScene(game);
            break;

        default:
            break;
	}
}

void RainClouds::saveSettingData()
{
    auto rootDic = Dictionary::create();
    auto bestDistanceScoreFloat = Float::create(bestDistance);
    auto currentDistanceScoreFloat = Float::create(totalDistance);
    auto isBestInt = Integer::create(isBest);
    
    rootDic->setObject(bestDistanceScoreFloat, "key_bestDistance");
	rootDic->setObject(currentDistanceScoreFloat, "key_currentDistance");
	rootDic->setObject(isBestInt, "key_isBest");
    
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
	std::string fullPath = writablePath + SAVE_FILE;
	rootDic->writeToFile(fullPath.c_str());
}

void RainClouds::loadSettingData()
{
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + SAVE_FILE;
    
	auto loadDict = __Dictionary::createWithContentsOfFile(fullPath.c_str());
	auto floatValue = (__String*)loadDict->objectForKey("key_bestDistance");
    
	if (floatValue != NULL) {
		bestDistance = std::atof(floatValue->getCString());
	} else {
		bestDistance = 0.0f;
	}
    
}