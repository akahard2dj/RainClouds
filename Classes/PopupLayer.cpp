#include "PopupLayer.h"
#include "GameDefinition.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AdmobHelperBridge.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

USING_NS_CC;
bool PopLayer::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0)))	{
		return false;
	}
    
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_CLICK);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0.3);
    
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

	auto index = __Integer::create(0);
	NotificationCenter::getInstance()->postNotification("NotificationPopup", index);

	winSize = Director::getInstance()->getWinSize();

	auto menuItem1 = MenuItemImage::create("labelTrans.png",
		"labelTrans.png",
		CC_CALLBACK_1(PopLayer::doCloseLayer, this));
	auto menuItem2 = MenuItemImage::create("labelTrans.png",
		"labelTrans.png",
		CC_CALLBACK_1(PopLayer::doCloseLayer, this));
	auto menuItem3 = MenuItemImage::create("labelTrans.png",
		"labelTrans.png",
		CC_CALLBACK_1(PopLayer::doCloseLayer, this));

	char buf[256];

	// Resume
	//menuItem1->setPosition(Point(winSize.width / 2, winSize.height*0.7));
	menuItem1->setTag(1);
	//auto menu1 = Menu::create(menuItem1, NULL);
	//menu1->setPosition(Point::ZERO);
	//this->addChild(menu1, Z_GAME);
	sprintf(buf, "%s", "Resume");
	
	auto label1 = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	label1->setPosition(winSize.width / 2, winSize.height*0.7);
	label1->setColor(Color3B(255, 255, 255));
	this->addChild(label1, Z_GAME + 1);

	// Restart
	//menuItem2->setPosition(Point(winSize.width / 2, winSize.height / 2));
	menuItem2->setTag(2);
	//auto menu2 = Menu::create(menuItem2, NULL);
	//menu2->setPosition(Point::ZERO);
	//this->addChild(menu2, 10);
	sprintf(buf, "Restart");
	
	auto label2 = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	label2->setPosition(Point(winSize.width / 2, winSize.height / 2));
	label2->setColor(Color3B(255, 255, 255));
	this->addChild(label2, Z_GAME + 1);

	// Exit
	//menuItem3->setPosition(Point(winSize.width / 2, winSize.height*0.3));
	menuItem3->setTag(3);
	//auto menu3 = Menu::create(menuItem3, NULL);
	//menu3->setPosition(Point::ZERO);
	//this->addChild(menu3, 10);
	sprintf(buf, "Exit");
	
	auto label3 = LabelTTF::create(buf, "fonts/Chalkduster.ttf", 50);
	label3->setPosition(Point(winSize.width / 2, winSize.height*0.3));
	label3->setColor(Color3B(255, 255, 255));
	this->addChild(label3, Z_GAME + 1);

	auto pMenu = Menu::create(menuItem1, menuItem2, menuItem3, NULL);
	pMenu->setPosition(Vec2::ZERO);
	menuItem1->setPosition(winSize.width/2, winSize.height*0.7);
	menuItem2->setPosition(winSize.width/2, winSize.height/2);
	menuItem3->setPosition(winSize.width/2, winSize.height*0.3);
	//pMenu->alignItemsVertically();
	this->addChild(pMenu);

	// Background Layer
	backLayer = LayerColor::create(Color4B(0, 0, 0, 200), winSize.width, winSize.height);
	backLayer->setAnchorPoint(Point(0, 0));
	backLayer->setPosition(Point(0, 0));
	this->addChild(backLayer, Z_GAME - 3);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(PopLayer::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(PopLayer::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	return true;
}

void PopLayer::doCloseLayer(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_CLICK);
	MenuItem* pMenuItem = (MenuItem*)pSender;
	int tag = pMenuItem->getTag();

	__Integer* tagNumber = NULL;
	tagNumber = __Integer::create(tag);
	NotificationCenter::getInstance()->postNotification("NotificationPopup", tagNumber);
	this->removeFromParentAndCleanup(true);
}

bool PopLayer::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void PopLayer::onTouchEnded(Touch* touch, Event* event)
{
	//nothing..
}
