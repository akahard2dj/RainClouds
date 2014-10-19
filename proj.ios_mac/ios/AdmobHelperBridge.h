//
//  AdmobHelperBridge.h
//  RainCloud
//
//  Created by Dowon Yi on 2014. 7. 26..
//
//

class AdmobHelperBridge
{
public:
    AdmobHelperBridge();
    static AdmobHelperBridge* shared();
    virtual void showAD();
    virtual void hideAD();
    virtual bool getIsBannered();
};