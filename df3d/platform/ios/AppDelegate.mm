//
//  AppDelegate.mm
//  df3d
//

#import "AppDelegate.h"
#import "GameViewController.h"

#import <libdf3d/df3d.h>
#import <libdf3d/platform/AppDelegate.h>

static IOSAppState g_appState;

namespace df3d {

void Application::setupDelegate(unique_ptr<AppDelegate> appDelegate)
{
    g_appState.appDelegate = std::move(appDelegate);
}

void Application::setTitle(const std::string &title)
{

}

}

IOSAppState& GetIOSAppState()
{
    return g_appState;
}

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    g_appState.engine.reset(new df3d::EngineController());

    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.backgroundColor = [UIColor blackColor];
    self.window.rootViewController = [[[GameViewController alloc] initWithNibName:nil bundle:nil] autorelease];
    [self.window makeKeyAndVisible];

    return YES;
}

- (void)dealloc
{
    [self.window release];
    [super dealloc];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    GetIOSAppState().appDelegate->onAppWillResignActive();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    GetIOSAppState().appDelegate->onAppDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    GetIOSAppState().appDelegate->onAppWillEnterForeground();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    GetIOSAppState().appDelegate->onAppDidBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    g_appState.appDelegate->onAppEnded();

    g_appState.engine->shutdown();
}

@end
