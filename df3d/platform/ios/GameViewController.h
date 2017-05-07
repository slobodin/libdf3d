//
//  GameViewController.h
//  df3d
//

#import <UIKit/UIKit.h>

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
@end
