//
// MyBodyParser.h
// RainCloud
//
// Created by Dowon Yi on 2014. 8. 14..
//
//

#ifndef __RainCloud__MyBodyParser__
#define __RainCloud__MyBodyParser__
#pragma once

#include <string>
#include "cocos2d.h"
#include "json/document.h"

class MyBodyParser 
{
	MyBodyParser(){}
	rapidjson::Document doc;
public:
	static MyBodyParser* getInstance();
	bool parseJsonFile(const std::string& pFile);
	bool parse(unsigned char* buffer, long length);
	void clearCache();
	cocos2d::PhysicsBody* bodyFormJson(cocos2d::Node* pNode, const std::string& name, cocos2d::PhysicsMaterial material);
};

#endif /* defined(__RainCloud__MyBodyParser__) */