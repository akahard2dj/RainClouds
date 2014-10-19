#include "CItem.h"

int Item::_itemType = 0;
int Item::_cloudType = 0;
int Item::_count = 0;

Item::Item()
{
}
Item::~Item()
{
}
Item* Item::create(const char* pszFileName)
{
	Item* sprite = new Item();
	sprite->initWithFile(pszFileName);
	return sprite;
}