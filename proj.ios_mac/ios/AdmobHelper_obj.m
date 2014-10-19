//
//  admob_obj.m
//  RainCloud
//
//  Created by Dowon Yi on 2014. 7. 23..
//
//

#import "AdmobHelper_obj.h"
#import "CCEAGLView.h"
#import "GADBannerViewDelegate.h"

@implementation AdmobHelper 

#pragma mark init/dealloc

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Use predefined GADAdSize constants to define the GADBannerView.
    self.adBanner = [[GADBannerView alloc] initWithAdSize:kGADAdSizeBanner];
    self.adBanner.center = CGPointMake(self.view.center.y,
                                       self.view.frame.size.width-self.adBanner.frame.size.height/2);
    
    // Note: Edit SampleConstants.h to provide a definition for kSampleAdUnitID before compiling.
    self.adBanner.adUnitID = ADUNITID;
    self.adBanner.delegate = self;
    self.adBanner.rootViewController = self;
    [self.view addSubview:self.adBanner];
    [self.adBanner loadRequest:[self request]];
}

- (void)dealloc {
    [super dealloc];
    _adBanner.delegate = nil;
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskPortrait;
}

#pragma mark GADRequest generation

- (GADRequest *)request {
    GADRequest *request = [GADRequest request];
    
    // Make the request for a test ad. Put in an identifier for the simulator as well as any devices
    // you want to receive test ads.
    request.testDevices = @[
                            // TODO: Add your device/simulator test identifiers here. Your device identifier is printed to
                            // the console when the app is launched.
                            GAD_SIMULATOR_ID
                            ];
    return request;
}

#pragma mark GADBannerViewDelegate implementation

// We've received an ad successfully.
- (void)adViewDidReceiveAd:(GADBannerView *)adView {
    NSLog(@"Received ad successfully");
}

- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error {
    NSLog(@"Failed to receive ad with error: %@", [error localizedFailureReason]);
}

@end