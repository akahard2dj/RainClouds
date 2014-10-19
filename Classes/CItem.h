#ifndef __RainCloud__Item__
#define __RainCloud__Item__

#include "cocos2d.h"
class Item : public cocos2d::Sprite
{
private:
public:
	Item();
	~Item();
	static Item* create(const char* pszFileName);
	int getItemType() { return _itemType; }
	int count;
public:
	static int _itemType;
	static int _cloudType;
	static int _count;
};
#endif /* defined(__RainCloud__CItem__) */