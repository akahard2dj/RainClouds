/*#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

///< iTunes Connect에 설정한 리더보드 Id
#define LeaderboardID @"RainCloudsLBId"

//@interface GameCenter : UIViewController <GKLeaderboardViewControllerDelegate>
@interface GameCenter : UIViewController <GKGameCenterControllerDelegate>
+ (bool) startGameCenter;
+ (BOOL) isGameCenterAPIAvailable;
+ (void) authenticateLocalPlayer;
+ (void) reportScore:(int64_t)score;
+ (void) showGameCenter;
+ (void) getScore;
+ (void) leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController;

@end*/

#import <Foundation/Foundation.h>
#import <UIKit/UiKit.h>
#import <GameKit/GameKit.h>

#define LeaderboardID @"RainCloudsLBId"

@interface GameKitHelper : NSObject <GKLeaderboardViewControllerDelegate>{
    BOOL userAuthenticated;
}

@property (nonatomic) BOOL gameCenterAvailable;
@property (nonatomic) int64_t highScore;
@property (nonatomic) BOOL highScoreFetchedOK;

+ (GameKitHelper *)sharedGameKitHelper;
- (void) authenticateLocalUser;
- (void) showLeaderboard;
- (void) leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController;
- (void) reportScore:(int64_t)score;
- (void) receiveScore;
- (int64_t) getHighScore;

@end