#ifndef __RainCloud__StartRainCloudScene__
#define __RainCloud__StartRainCloudScene__

#include "cocos2d.h"
#include "CRandom.h"

class StartRainCloud : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void menuStartCallback(cocos2d::Ref* pSender);
	void menuSelectCallback(cocos2d::Ref* pSender);
	void menuGameCenterCallback(cocos2d::Ref* pSender);
	CREATE_FUNC(StartRainCloud);

private:
	float bestDistanceScore;
	float curreuntDistanceScore;
	int isBest;

	Random* random;
	
	cocos2d::Size winSize;
	cocos2d::Sprite* makeCloud(cocos2d::Point point);
	void removeCloud(cocos2d::Sprite* cloud);
	void ufoAnimation(float dt);
	void cloudAnimation(float dt);
	void rainEmitter(float dt);
	void checkSaveFile();
	void saveSettingData();
};
#endif /* defined(__RainCloud__StartRainCloudScene__) */