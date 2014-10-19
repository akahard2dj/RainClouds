#ifndef __RainCloud__PopupLayer__
#define __RainCloud__PopupLayer__

#include "cocos2d.h"

class PopLayer : public cocos2d::LayerColor
{
public:
	virtual bool init();
	CREATE_FUNC(PopLayer);

private:
	cocos2d::Size winSize;
	cocos2d::LayerColor* backLayer;
	cocos2d::LayerColor* popUpLayer;
	void doCloseLayer(cocos2d::Ref* pSender);
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
};


#endif /* defined(__RainCloud__PopupLayer__) */