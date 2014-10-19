#include "StartScene.h"
#include "StartRainCloudScene.h"

USING_NS_CC;

Scene* StartScene::createScene()
{
	auto scene = Scene::create();
	auto layer = StartScene::create();
	scene->addChild(layer);

	return scene;
}

bool StartScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto logo = Sprite::create("logo.png");
	logo->setPosition(visibleSize.width / 2, visibleSize.height / 2);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	float verticalRatio = visibleSize.height / 640.0;
	float horizaonRatio = visibleSize.width / 1136.0;
	float scaleFactor = MAX(verticalRatio, horizaonRatio);
	logo->setScale(scaleFactor);
#endif
	this->addChild(logo);

	auto fadeIn = FadeIn::create(1.0);
	auto fadeOut = FadeOut::create(1.0);
	auto jumpScene = CallFunc::create(CC_CALLBACK_0(StartScene::replaceScene, this));
	auto delay1 = DelayTime::create(1.0);
	auto delay2 = DelayTime::create(0.7);
	//auto jumpScene = CallFunc::create(CC_CALLBACK_0(StartScene::replaceScene, this, NULL));
	auto action = Sequence::create(fadeIn, delay1, fadeOut, delay2, jumpScene, NULL);
	logo->runAction(action);
	return true;
}
void StartScene::replaceScene()
{
	Scene* game = StartRainCloud::createScene();
	Director::getInstance()->replaceScene(game);
}