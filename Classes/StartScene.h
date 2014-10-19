#ifndef __RainCloud__StartScene__
#define __RainCloud__StartScene__

#include "cocos2d.h"

class StartScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(StartScene);

private:
	void replaceScene();
};
#endif /* defined(__RainCloud__StartScene__) */