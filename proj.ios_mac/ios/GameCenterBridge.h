//
//  GameCenterBridge.h
//  RainCloud
//
//  Created by Dowon Yi on 2014. 7. 19..
//
//

#ifndef __RainCloud__GameCenterBridge__
#define __RainCloud__GameCenterBridge__

class GameCenterBridge
{
public:
    static float score;
    GameCenterBridge();
    static GameCenterBridge* shared();
    virtual void showGameCenter();
    virtual void reportScore(long long score);
    virtual void startGameCenter();
    virtual long long getScore();
};
#endif /* defined(__RainCloud__GameCenterBridge__) */
