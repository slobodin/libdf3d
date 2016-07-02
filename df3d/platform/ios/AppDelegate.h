//
//  AppDelegate.h
//  df3d
//

#import <UIKit/UIKit.h>

#import <libdf3d/df3d.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

struct IOSAppState
{
    unique_ptr<df3d::AppDelegate> appDelegate;
};

IOSAppState& GetIOSAppState();
