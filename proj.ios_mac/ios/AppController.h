#import <UIKit/UIKit.h>
#import "GADBannerView.h"
#import "AdamAdView.h"

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate, GADBannerViewDelegate, AdamAdViewDelegate> {
    UIWindow *window;
    GADBannerView *_bannerView;
    AdamAdView *_adamAdView;
    BOOL isAdamActivated;
    BOOL isBannered;
}

@property(nonatomic, readonly) RootViewController* viewController;

- (void)showAD;
- (void)hideAD;
- (BOOL)getIsBannered;

@end

