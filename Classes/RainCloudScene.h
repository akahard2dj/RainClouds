//
// RainCloudScene.h
// RainCloud
//
// Created by Dowon Yi on 2014. 8. 14..
//
//
#ifndef __RainCloud__RainCloudScene__
#define __RainCloud__RainCloudScene__

#include "cocos2d.h"

#include "CItem.h"
#include "CRandom.h"

#include "StartRainCloudScene.h"
#include "PopupLayer.h"
#include "EndLayer.h"

#include "GameDefinition.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "editor-support\cocostudio\CocoStudio.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "../cocos2d/cocos/editor-support/cocostudio/CocoStudio.h"
#include "GameCenterBridge.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "cocostudio/CocoStudio.h"
#endif

class RainClouds : public cocos2d::LayerColor
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(RainClouds);

	void optionOpenCallback(cocos2d::Ref* pSender);

private:
	// Initialization
	bool isUFODataLoaded;
	bool isTouched;
	bool isStarted;
	bool isUpperTouched;
    bool isLowerTouched;
	bool isFull;
	bool isEnded;

	cocostudio::Armature* UFOanim;

	void initGame();
	void initAnimation();
    void initSound();
	void onUFOAnimDataLoaded(float dt);
	void onStarAnimDataLoaded(float dt);

	// Physics Engine
	cocos2d::PhysicsWorld* m_world;
	void setPhysicsWorld(cocos2d::PhysicsWorld* world) { m_world = world; }
	bool onContactBegin(cocos2d::PhysicsContact& contact);
	void onContactSeperate(cocos2d::PhysicsContact& contact);

	// Event
	cocos2d::EventDispatcher* eventDispatcher;
	void selectSpecialEventListener();
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	
	// Animation
	void updateSchedule();

    void showTouchStart(float dt);
	void showEndLayer(float dt);
	void updateDistance(float dt);
	void updateDistanceLabel(float dt);
	void updateStarTime(float dt);
	void updateCloudTime(float dt);
	void ufoAnimation(float dt);
	void addCloud(float dt);
	void addStar(float dt);

	void starAnimation(cocos2d::Point point);
	void updateHP(float amount);
	void updateMP(float amount);

	void unscheduleAll();
	
	void rainEmitter();
	void tempRainEmitter(cocos2d::Point point);

	float showingCloudInterval;

	// Notification
	void doNotification(cocos2d::Ref *obj);

	//ETC
	cocos2d::Size winSize;
	Random* random;
	cocos2d::LabelTTF* labelForDistance;

	cocos2d::Sprite* makeBorder(cocos2d::Point point);
	cocos2d::Sprite* makeCloud(cocos2d::Point point);
	cocos2d::Sprite* makeStar(cocos2d::Point point);
	void removeItem(cocos2d::Sprite* item);
    void playSound(std::string fileName);

    void loadSettingData();
    void saveSettingData();
    
	cocos2d::Sprite* status;
	cocos2d::Sprite* statusStar;
	cocos2d::Sprite* starTouch;
    cocos2d::Sprite* touchStart;
	cocostudio::Armature* starAnim;
    
    int isBest;
    float bestDistance;
	float totalDistance;

	float HP;
	float MP;
	float nRowGrid;
};


#endif /* defined(__RainCloud__RainCloudScene__) */