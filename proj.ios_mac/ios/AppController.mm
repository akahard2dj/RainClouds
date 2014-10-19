/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#import "AppController.h"
#import "CCEAGLView.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "RootViewController.h"
#import "GameCenter_obj.h"

#define ADAMCLIENTID    @"97d1Z01T147877d1ff4"
#define ADMOBUNITID     @"ca-app-pub-4871252957066311/9123409783"

@implementation AppController

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    

    // Override point for customization after application launch.

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];

    // Init the CCEAGLView
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGBA8
                                     depthFormat: GL_DEPTH24_STENCIL8_OES
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples: 0];

    // Use RootViewController manage CCEAGLView 
    _viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    //_viewController.wantsFullScreenLayout = YES;
    _viewController.view = eaglView;

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: _viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:_viewController];
    }

    [window makeKeyAndVisible];

    [[UIApplication sharedApplication] setStatusBarHidden:true];

    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    cocos2d::GLView *glview = cocos2d::GLView::createWithEAGLView(eaglView);
    cocos2d::Director::getInstance()->setOpenGLView(glview);

    cocos2d::Application::getInstance()->run();
    
    // GameCenter
    [[GameKitHelper sharedGameKitHelper] authenticateLocalUser];

    [self initAdBanner];

    return YES;
}

- (void)initAdBanner
{
    // Initailly, AdBanner will load ADAM, If failed, flag is changed.
    isAdamActivated = YES;
    //ADAM
    _adamAdView = [AdamAdView sharedAdView];
    _adamAdView.frame = CGRectMake(0.0f, 0.0f, 320.0f, 50);
    _adamAdView.center =CGPointMake(_viewController.view.frame.size.width/2,
                                    _viewController.view.frame.size.height-50/2);
    
    [[AdamAdView sharedAdView] setDelegate:self];
    _adamAdView.clientId = ADAMCLIENTID;
    
    [_viewController.view addSubview:_adamAdView];
    if (!_adamAdView.usingAutoRequest) {
        [_adamAdView startAutoRequestAd:60.0];
    }
    _adamAdView.hidden = YES;
    
    // Admob
    CGPoint origin = CGPointMake(_viewController.view.frame.size.width/2 - 320/2,
                                 _viewController.view.frame.size.height - 50);
    _bannerView = [[GADBannerView alloc] initWithAdSize:kGADAdSizeBanner origin:origin];
    //_bannerView.center = CGPointMake(_viewController.view.center.y, _viewController.view.frame.size.width-_bannerView.frame.size.height/2);
    
    _bannerView.delegate = self;
    _bannerView.adUnitID = ADMOBUNITID;
    
    _bannerView.rootViewController = _viewController;
    [_viewController.view addSubview:_bannerView];

    _bannerView.hidden = YES;
    
    isBannered = NO;
    
}

- (GADRequest *)requestAdmob
{
    GADRequest *request;
    request = [GADRequest request];
    NSLog(@"requested");
    return request;
}

- (void)showAD
{
    //
    if (isAdamActivated == YES) {
        _adamAdView.hidden = NO;
        if ([_adamAdView hasAd] == NO) {
            isAdamActivated = NO;
        }
        NSLog(@"Adam is activated");
    }
    //
    else {
        _bannerView.hidden = NO;
        GADRequest *request = [self requestAdmob];
        //request.testDevices = [NSArray arrayWithObjects:GAD_SIMULATOR_ID, nil];
        [_bannerView loadRequest:request];
        NSLog(@"Admob is activated");
    }
    
    isBannered = YES;
}

- (void)hideAD
{
    if (isAdamActivated == YES) {
        _adamAdView.hidden = YES;
    } else {
        _bannerView.hidden =YES;
    }
    
    isBannered = NO;
}

- (BOOL)getIsBannered
{
    return isBannered;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
     //We don't need to call this method any more. It will interupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->pause(); */
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
     //We don't need to call this method any more. It will interupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->resume(); */
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    cocos2d::Application::getInstance()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    cocos2d::Application::getInstance()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
    [window release];
    [super dealloc];
}


@end
