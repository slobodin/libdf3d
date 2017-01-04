//
//  GameViewController.m
//  df3d
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>

#import <df3d/df3d.h>
#import "AppDelegate.h"

namespace df3d {

extern bool EngineInit(EngineInitParams params);

}

@interface GameViewController () {

}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GameViewController {
    CGFloat contentScaleFactor;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    contentScaleFactor = [UIScreen mainScreen].scale;

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (self.context)
    {
        GLKView *view = (GLKView *)self.view;
        view.multipleTouchEnabled = true;
        view.context = self.context;
        view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
        view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
//        [view setDrawableMultisample:GLKViewDrawableMultisample4X];

        [self setupGL];
    }
    else
    {
        NSLog(@"Failed to create ES context");
    }
}

- (void)dealloc
{
    [self tearDownGL];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }

    [self.context release];

    [super dealloc];
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
        [self.context release];
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setupGL
{
    self.preferredFramesPerSecond = 60.0f;
    [EAGLContext setCurrentContext:self.context];

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

    assert(df3d::AppDelegate::getInstance() != nullptr);

    auto engineInitParams = df3d::AppDelegate::getInstance()->getInitParams();
    engineInitParams.windowWidth = screenWidth;
    engineInitParams.windowHeight = screenHeight;

    df3d::EngineInit(engineInitParams);

    if (!df3d::AppDelegate::getInstance()->onAppStarted())
        DFLOG_CRITICAL("Game code initialization failed");
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    df3d::svc().step();
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;
        auto pointerId = reinterpret_cast<uintptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::DOWN);
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;
        auto pointerId = reinterpret_cast<uintptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::MOVING);
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;
        auto pointerId = reinterpret_cast<uintptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::UP);
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        point.x *= self->contentScaleFactor;
        point.y *= self->contentScaleFactor;
        auto pointerId = reinterpret_cast<uintptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, df3d::Touch::State::CANCEL);
    }
}

@end
