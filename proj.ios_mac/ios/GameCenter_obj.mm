#import "GameCenter_obj.h"

@implementation GameKitHelper

@synthesize gameCenterAvailable;
@synthesize highScore = _highScore;
@synthesize highScoreFetchedOK = _highScoreFetchedOK;

static GameKitHelper *sharedHelper = nil;
static UIViewController* currentModalViewController = nil;

+ (GameKitHelper *)sharedGameKitHelper
{
    if (!sharedHelper) {
        sharedHelper = [[GameKitHelper alloc] init];
    }
    
    return sharedHelper;
}

- (BOOL)isGameCenterAvailable
{
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    
    NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer
                                           options:NSNumericSearch] != NSOrderedAscending);
    
    return (gcClass && osVersionSupported);
}

- (id)init
{
    if ((self = [super init])) {
        gameCenterAvailable = [self isGameCenterAvailable];
        if (gameCenterAvailable) {
            NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
            [nc addObserver:self
                   selector:@selector(authenticationChanged)
                       name:GKPlayerAuthenticationDidChangeNotificationName
                     object:nil];
        }
    }
    return self;
}

- (void)authenticationChanged
{
    if ([GKLocalPlayer localPlayer].isAuthenticated && !userAuthenticated) {
        NSLog(@"Authentication changed: player authenticated.");
        userAuthenticated = TRUE;
        [self receiveScore];
    } else if (![GKLocalPlayer localPlayer].isAuthenticated && userAuthenticated) {
        NSLog(@"Authentication changed: player not authenticated.");
        userAuthenticated = FALSE;
        self.highScoreFetchedOK = NO;
        self.highScore = 0;
    }
}

- (void)authenticateLocalUser
{
    if (!gameCenterAvailable) return;
    
    NSLog(@"Authenticating local user...");
    if ([GKLocalPlayer localPlayer].authenticated == NO) {
        [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:nil];
        //[[GKLocalPlayer localPlayer] setAuthenticateHandler:nil];
        [self receiveScore];
    } else {
        NSLog(@"Already authenticated");
    }
}

- (void)showLeaderboard
{
    if (!gameCenterAvailable) return;
    
    GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];
    if (leaderboardController != nil) {
        leaderboardController.leaderboardDelegate = self;
        
        UIWindow *window = [[UIApplication sharedApplication] keyWindow];
        currentModalViewController = [[UIViewController alloc] init];
        [window addSubview:currentModalViewController.view];
        [currentModalViewController presentModalViewController:leaderboardController animated:YES];
    }
}

- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
    if(currentModalViewController !=nil){
        [currentModalViewController dismissModalViewControllerAnimated:NO];
        [currentModalViewController release];
        [currentModalViewController.view removeFromSuperview];
        currentModalViewController = nil;
    }
}

- (void)reportScore:(int64_t)score
{
    if (!gameCenterAvailable) return;
    
    GKScore *scoreReporter = [[[GKScore alloc] initWithLeaderboardIdentifier:LeaderboardID] autorelease];
    scoreReporter.value = score;
    [scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
        if (error != nil)
        {
            // handle the reporting error
            NSLog(@"Error.");
            //If your application receives a network error, you should not discard the score.
            //Instead, store the score object and attempt to report the player’s process at
            //a later time.
        }else {
            NSLog(@"Complete");
        }
    }];
}

- (void)receiveScore
{
    if (!gameCenterAvailable)
        
    self.highScoreFetchedOK = NO;
    self.highScore = 0;
    
    GKLeaderboard *req = [[GKLeaderboard alloc] init];
    if (req != nil) {
        req.identifier = LeaderboardID;
        [req loadScoresWithCompletionHandler:^(NSArray *scores, NSError *error) {
            if (error != nil) {
                NSLog(@"Error");
            } else {
                [self localPlayerScoreReceived:req.localPlayerScore];
                NSLog(@"Complete getting score");
            }
        }];
    }
}

- (void)localPlayerScoreReceived:(GKScore *)score
{
    self.highScore = score.value;
    self.highScoreFetchedOK = YES;
    NSLog(@"Best Score : %lld", self.highScore);
}

- (int64_t)getHighScore
{
    return self.highScore;
}
@end







