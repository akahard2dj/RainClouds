//
//  AdmobHelperBridge.m
//  RainCloud
//
//  Created by Dowon Yi on 2014. 7. 26..
//
//

#include "AdmobHelperBridge.h"
#import "AppController.h"

static void static_showAD()
{
    AppController *app = (AppController *)[UIApplication sharedApplication].delegate;
    [app showAD];
}

static void static_hideAD()
{
    AppController *app = (AppController *)[UIApplication sharedApplication].delegate;
    [app hideAD];
}

static bool static_getIsBannered()
{
    AppController *app = (AppController *)[UIApplication sharedApplication].delegate;
    BOOL isBannered = [app getIsBannered];
    bool returnFlag;
    if (isBannered == YES)
        returnFlag = true;
    if (isBannered == NO)
        returnFlag = false;
    
    return returnFlag;
}

static AdmobHelperBridge *s_pBridge;

AdmobHelperBridge::AdmobHelperBridge()
{
    
}

AdmobHelperBridge* AdmobHelperBridge::shared()
{
    if(!s_pBridge) {
        s_pBridge = new AdmobHelperBridge();
    }
    
    return s_pBridge;
}

void AdmobHelperBridge::showAD()
{
    static_showAD();
}

void AdmobHelperBridge::hideAD()
{
    static_hideAD();
}

bool AdmobHelperBridge::getIsBannered()
{
    return static_getIsBannered();
}