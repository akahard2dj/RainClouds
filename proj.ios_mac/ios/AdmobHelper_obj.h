//
//  admob_obj.h
//  RainCloud
//
//  Created by Dowon Yi on 2014. 7. 23..
//
//

#import "AppController.h"
#import "GADBannerView.h"
#import "GADBannerViewDelegate.h"
#import "GADRequest.h"

#define ADUNITID @"ca-app-pub-4871252957066311/2296963786"

@class GADBannerView;
@class GADRequest;

@interface AdmobHelper : UIViewController<GADBannerViewDelegate>

@property (nonatomic, strong) GADBannerView *adBanner;
//+ (AdmobHelper *)sharedHelper;
//- (instancetype) initWithContentViewController:(UIViewController *)viewController;

- (GADRequest *)request;

@end