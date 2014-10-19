#ifndef __RainCloud__endLayer__
#define __RainCloud__endLayer__

#include "cocos2d.h"

class EndLayer : public cocos2d::LayerColor
{
public:
	virtual bool init();
	CREATE_FUNC(EndLayer);

private:
	float bestDistanceScore;
	float currentDistanceScore;
	int isBest;

	cocos2d::Size winSize;
	cocos2d::LayerColor* backLayer;
	cocos2d::LayerColor* popUpLayer;
	void doCloseLayer(cocos2d::Ref* pSender);
	void loadSettingData();
    
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
	void doStamp(float dt);
    char stampName[100];
};

#endif /* defined(__RainCloud__endLayer__) */