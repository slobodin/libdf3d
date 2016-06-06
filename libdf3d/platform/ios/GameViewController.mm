//
//  GameViewController.m
//  df3d
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>

#import <libdf3d/df3d.h>
#import <libdf3d/platform/AppDelegate.h>

namespace df3d {

struct IOSAppState
{
    unique_ptr<df3d::AppDelegate> appDelegate;
    unique_ptr<df3d::EngineController> engine;
};

static IOSAppState g_appState;

void Application::setupDelegate(unique_ptr<AppDelegate> appDelegate)
{
    g_appState.appDelegate = std::move(appDelegate);
}

void Application::setTitle(const std::string &title)
{

}

EngineController& svc() { return *g_appState.engine; }
    
}

@interface GameViewController () {

}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GameViewController {
    CGFloat contentScaleFactor;
    df3d::TouchID primaryTouchId;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self->contentScaleFactor = [UIScreen mainScreen].scale;
    self->primaryTouchId = df3d::Touch::INVALID_ID;

    df3dInitialized();

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }

    GLKView *view = (GLKView *)self.view;
    view.multipleTouchEnabled = true;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;

    [self setupGL];
}

- (void)dealloc
{
    [self tearDownGL];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;

        [self tearDownGL];

        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setupGL
{
    self.preferredFramesPerSecond = 30.0f;
    [EAGLContext setCurrentContext:self.context];

    df3d::g_appState.engine.reset(new df3d::EngineController());

    int screenWidth, screenHeight;
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

    auto engineInitParams = df3d::g_appState.appDelegate->getInitParams();
    engineInitParams.windowWidth = screenWidth;
    engineInitParams.windowHeight = screenHeight;

    df3d::g_appState.engine->initialize(engineInitParams);
    if (!df3d::g_appState.appDelegate->onAppStarted())
        DFLOG_CRITICAL("Game code initialization failed");
}

- (void)tearDownGL
{
    df3d::g_appState.appDelegate->onAppEnded();
    df3d::g_appState.engine->shutdown();

    [EAGLContext setCurrentContext:self.context];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    df3d::g_appState.engine->step();
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
