//
//  GameViewController.h
//  df3d
//

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>
#ifndef DF3D_APPLETV
#import <CoreMotion/CoreMotion.h>
#endif

#ifdef DF3D_APPLETV
#import <GameController/GameController.h>
#define GameViewControllerBase GCEventViewController
#else
#define GameViewControllerBase UIViewController
#endif

@interface DF3DView : MTKView<MTKViewDelegate>

- (void) startupEngine;

@end

@interface GameViewController : GameViewControllerBase

#ifndef DF3D_APPLETV

- (void) startAccelerometerListener;
- (void) stopAccelerometerListener;

@property (assign, nonatomic) BOOL listeningAccelerometer;

@property (strong, nonatomic) CMMotionManager *motionManager;
@property (strong, nonatomic) NSOperationQueue *motionQueue;

#endif

@end
