//
//  GameViewController.h
//  df3d
//

#import <UIKit/UIKit.h>
#ifndef DF3D_APPLETV
#import <CoreMotion/CoreMotion.h>
#endif

#ifdef DF3D_APPLETV
#import <GameController/GameController.h>
#define GameViewControllerBase GCEventViewController
#else
#define GameViewControllerBase UIViewController
#endif

@interface OpenGLView : UIView
@end

@interface GameViewController : GameViewControllerBase
@property (nonatomic, retain) OpenGLView* openglView;

#ifndef DF3D_APPLETV

- (void) startAccelerometerListener;
- (void) stopAccelerometerListener;

@property (assign, nonatomic) BOOL listeningAccelerometer;

@property (strong, nonatomic) CMMotionManager *motionManager;
@property (strong, nonatomic) NSOperationQueue *motionQueue;

#endif

@end
