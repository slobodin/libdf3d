//
//  GameViewController.m
//  df3d
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>

#import <libdf3d/df3d.h>
#import "GLView.h"
#import "AppDelegate.h"

@interface GameViewController () {

}

@end

@implementation GameViewController {
    CGFloat contentScaleFactor;
    df3d::TouchID primaryTouchId;
    int screenWidth;
    int screenHeight;
}

- (void)viewDidLoad
{
    primaryTouchId = df3d::Touch::INVALID_ID;
    contentScaleFactor = [[UIScreen mainScreen] scale];

    // Get screen size.
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0)
    {
        CGFloat screenScale = [[UIScreen mainScreen] scale];
        CGSize size = [[UIScreen mainScreen] bounds].size;
        screenWidth = size.width * screenScale;
        screenHeight = size.height * screenScale;
    }
    else
    {
        CGSize size = [[UIScreen mainScreen] nativeBounds].size;
        screenWidth = size.width;
        screenHeight = size.height;
    }

    if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation))
        std::swap(screenWidth, screenHeight);

    _glView = [[GLView alloc] initWithFrame:self.view.bounds];
    [self.view addSubview:_glView];

    [super viewDidLoad];

    df3dSetApplicationDelegate();

    auto engineInitParams = GetIOSAppState().appDelegate->getInitParams();
    engineInitParams.windowWidth = screenWidth;
    engineInitParams.windowHeight = screenHeight;

    GetIOSAppState().engine->initialize(engineInitParams);
    if (!GetIOSAppState().appDelegate->onAppStarted())
        DFLOG_CRITICAL("Game code initialization failed");
}

- (void)dealloc
{
    [_glView release];
    [super dealloc];
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;
        df3d::TouchID pointerId = reinterpret_cast<intptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::DOWN);

        if (self->primaryTouchId == df3d::Touch::INVALID_ID)
        {
            df3d::svc().inputManager().onMouseButtonPressed(df3d::MouseButton::LEFT, point.x, point.y);

            self->primaryTouchId = pointerId;
        }
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;
        df3d::TouchID pointerId = reinterpret_cast<intptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::MOVING);

        if (pointerId == self->primaryTouchId)
            df3d::svc().inputManager().setMousePosition(point.x, point.y);
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        df3d::TouchID pointerId = reinterpret_cast<intptr_t>(touch);
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::UP);

        if (pointerId == self->primaryTouchId)
        {
            df3d::svc().inputManager().onMouseButtonReleased(df3d::MouseButton::LEFT, point.x, point.y);

            self->primaryTouchId = df3d::Touch::INVALID_ID;
        }
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        df3d::TouchID pointerId = reinterpret_cast<intptr_t>(touch);
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::CANCEL);

        if (pointerId == self->primaryTouchId)
        {
            df3d::svc().inputManager().onMouseButtonReleased(df3d::MouseButton::LEFT, point.x, point.y);

            self->primaryTouchId = df3d::Touch::INVALID_ID;
        }
    }
}

@end
