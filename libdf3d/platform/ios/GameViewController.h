//
//  GameViewController.h
//  df3d
//

#import <UIKit/UIKit.h>

@class GLView;

namespace df3d { class AppDelegate; }

@interface GameViewController : UIViewController

@property (strong, nonatomic) GLView *glView;

@end
