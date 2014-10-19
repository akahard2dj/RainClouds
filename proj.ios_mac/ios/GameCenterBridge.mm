//
//  GameCenterBridge.cpp
//  RainCloud
//
//  Created by Dowon Yi on 2014. 7. 19..
//
//

#include "GameCenterBridge.h"
#import "GameCenter_obj.h"

static void static_showLeaderBoard()
{
    //[GameCenter showGameCenter];
    [[GameKitHelper sharedGameKitHelper] showLeaderboard];
}

static void static_reportScore(long long score)
{
    //[GameCenter reportScore:score];
    [[GameKitHelper sharedGameKitHelper] reportScore:score];
}

static void static_startGameCenter()
{
    //return [GameCenter startGameCenter];
    [[GameKitHelper sharedGameKitHelper] authenticateLocalUser];
}

static long long static_getScore()
{
//   [GameCenter getScore];
    return [[GameKitHelper sharedGameKitHelper] getHighScore];
}

static GameCenterBridge *s_pBridge;

GameCenterBridge::GameCenterBridge()
{
    
}

GameCenterBridge* GameCenterBridge::shared()
{
    if(!s_pBridge) {
        s_pBridge = new GameCenterBridge();
    }
    
    return s_pBridge;
}

void GameCenterBridge::showGameCenter()
{
    static_showLeaderBoard();
}

void GameCenterBridge::reportScore(long long score)
{
    static_reportScore(score);
}

void GameCenterBridge::startGameCenter()
{
    static_startGameCenter();
}

long long GameCenterBridge::getScore()
{
    return static_getScore();
}