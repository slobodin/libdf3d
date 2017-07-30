//
//  GameViewController.m
//  df3d
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>
#import <AVFoundation/AVFoundation.h>
#import <GameController/GameController.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import <df3d/df3d.h>
#import "AppDelegate.h"

static GameViewController* g_viewController;

namespace df3d {

extern bool EngineInit(EngineInitParams params);
// TODO: refactor this.
extern void AudioSuspend();
extern void AudioResume();

bool HandleControllerBackButtonPressed()
{
    if (auto l = df3d::svc().inputManager().getMfiControllerListener())
        return l->Mfi_buttonMenu_Pressed();

    return false;
}

}

@implementation OpenGLView {
    CADisplayLink* m_displayLink;
    CAEAGLLayer* m_eaglLayer;
    EAGLContext* m_context;
    GLuint m_framebuffer;
    GLuint m_colorBuffer;
    GLuint m_depthBuffer;
}

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)setupLayer {
    m_eaglLayer = (CAEAGLLayer*) self.layer;
    m_eaglLayer.opaque = YES;
}

- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    m_context = [[EAGLContext alloc] initWithAPI:api];
    if (!m_context) {
        NSLog(@"Failed to initialize OpenGLES 2.0 context");
        exit(1);
    }

    if (![EAGLContext setCurrentContext:m_context]) {
        NSLog(@"Failed to set current OpenGL context");
        exit(1);
    }
}

- (void)setupEngine
{
    CGSize size = [self getDisplaySize];

    assert(df3d::AppDelegate::getInstance() != nullptr);

    auto engineInitParams = df3d::AppDelegate::getInstance()->getInitParams();
    engineInitParams.windowWidth = size.width;
    engineInitParams.windowHeight = size.height;

    df3d::EngineInit(engineInitParams);

    if (!df3d::AppDelegate::getInstance()->onAppStarted())
        DFLOG_CRITICAL("Game code initialization failed");
}

- (void)setupDisplayLink {
    m_displayLink = [[CADisplayLink displayLinkWithTarget:self selector:@selector(renderFrame:)] retain];
    [m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (CGSize)getDisplaySize
{
    CGSize size = self.bounds.size;
    CGFloat scale = self.contentScaleFactor;
    size.width *= scale;
    size.height *= scale;
    return size;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
#ifndef DF3D_APPLETV
        self.multipleTouchEnabled = true;
#endif
        self.contentScaleFactor = [UIScreen mainScreen].scale;

        AVAudioSession *session = [AVAudioSession sharedInstance];
        BOOL success = FALSE;
        success = [session setCategory:AVAudioSessionCategoryAmbient error:nil];
        success = [session setActive:TRUE error:nil];

        [self setupLayer];
        [self setupContext];
        [self setupDisplayLink];
        [self createRenderbuffers:[self getDisplaySize]];
        [self setupEngine];

        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(applicationWillResignActive)
                                                     name:UIApplicationWillResignActiveNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(applicationDidBecomeActive)
                                                     name:UIApplicationDidBecomeActiveNotification
                                                   object:nil];

        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(controllerWasConnected:) name:GCControllerDidConnectNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(controllerWasDisconnected:) name:GCControllerDidDisconnectNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
        {
            if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan)
            {
                df3d::AudioSuspend();
            }
            else
            {
                DF3D_VERIFY([[AVAudioSession sharedInstance] setActive:TRUE error:nil]);

                df3d::AudioResume();
            }
        }];

        [self renderFrame:m_displayLink];
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [EAGLContext setCurrentContext:m_context];
    [self destroyRenderbuffers];

    [m_displayLink invalidate];
    [m_displayLink release];

    [EAGLContext setCurrentContext:nil];
    [m_context release];

    [super dealloc];
}

- (void)applicationWillResignActive
{
    [EAGLContext setCurrentContext:m_context];
    [self destroyRenderbuffers];

    [m_displayLink invalidate];
    [m_displayLink release];
    m_displayLink = nil;

    [EAGLContext setCurrentContext:nil];
}

- (void)applicationDidBecomeActive
{
    if (!m_displayLink) {
        m_displayLink = [[CADisplayLink displayLinkWithTarget:self selector:@selector(renderFrame:)] retain];
        [m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }

    [EAGLContext setCurrentContext:m_context];
    [self destroyRenderbuffers];
    [self createRenderbuffers:[self getDisplaySize]];
}

- (void)renderFrame:(CADisplayLink*)displayLink
{
    [EAGLContext setCurrentContext:m_context];

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    df3d::svc().step();

    glBindRenderbuffer(GL_RENDERBUFFER, m_colorBuffer);
    [m_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)createRenderbuffers:(CGSize)size
{
    int width = int(size.width);
    int height = int(size.height);

    NSLog(@"Creating renderbuffers for resolution %dx%d.", width, height);

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glGenRenderbuffers(1, &m_colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorBuffer);
    [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:m_eaglLayer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenRenderbuffers(1, &m_depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

- (void)destroyRenderbuffers
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (m_colorBuffer) {
        glDeleteRenderbuffers(1, &m_colorBuffer);
        m_colorBuffer = 0;
    }

    if (m_depthBuffer) {
        glDeleteRenderbuffers(1, &m_depthBuffer);
        m_depthBuffer = 0;
    }

    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
        m_framebuffer = 0;
    }
}

- (void)processTouches:(NSSet<UITouch*>*)touches withState:(df3d::Touch::State)state
{
#ifndef DF3D_APPLETV
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView: self];
        point.x *= self.contentScaleFactor;
        point.y *= self.contentScaleFactor;
        auto pointerId = reinterpret_cast<uintptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, state);
    }
#endif
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self processTouches:touches withState:df3d::Touch::State::DOWN];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self processTouches:touches withState:df3d::Touch::State::MOVING];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self processTouches:touches withState:df3d::Touch::State::UP];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self processTouches:touches withState:df3d::Touch::State::CANCEL];
}

- (void)controllerWasConnected:(NSNotification*)notification
{
    GCController *controller = (GCController *)notification.object;

    controller.playerIndex = GCControllerPlayerIndex1;

    NSLog(@"%@", [NSString stringWithFormat:@"Controller connected\nName: %@\n", controller.vendorName]);

    if (controller.gamepad)
    {
        // A, B, X, Y
        controller.gamepad.buttonA.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonA_Pressed(pressed);
        };
        controller.gamepad.buttonB.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonB_Pressed(pressed);
        };
        controller.gamepad.buttonX.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonX_Pressed(pressed);
        };
        controller.gamepad.buttonY.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonY_Pressed(pressed);
        };

        // Dpad
        controller.gamepad.dpad.left.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadLeft_Pressed(pressed);
        };
        controller.gamepad.dpad.right.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadRight_Pressed(pressed);
        };
        controller.gamepad.dpad.up.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadUp_Pressed(pressed);
        };
        controller.gamepad.dpad.down.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadDown_Pressed(pressed);
        };

        // shoulder buttons
        controller.gamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_LeftShoulder_Changed(value, pressed);
        };
        controller.gamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_RightShoulder_Changed(value, pressed);
        };
    }
    if (controller.extendedGamepad)
    {
        // two thumbsticks
        controller.extendedGamepad.leftThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_LeftThumbStick_Changed(xValue, yValue);
        };
        controller.extendedGamepad.rightThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_RightThumbStick_Changed(xValue, yValue);
        };

        controller.extendedGamepad.leftTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_LeftTrigger_Changed(value, pressed);
        };
        controller.extendedGamepad.rightTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_RightTrigger_Changed(value, pressed);
        };
    }

#ifndef DF3D_APPLETV
    controller.controllerPausedHandler = ^(GCController *controller) {
        df3d::HandleControllerBackButtonPressed();
    };
#endif

    int controllerId = reinterpret_cast<uintptr_t>(controller);
    if (controller.extendedGamepad == nil && controller.gamepad == nil)
        df3d::svc().inputManager().addController(controllerId, df3d::MFI_CONTROLLER_REMOTE);
    else
        df3d::svc().inputManager().addController(controllerId, df3d::MFI_CONTROLLER_GAMEPAD);

    [[UIApplication sharedApplication]setIdleTimerDisabled:YES];

    if (auto l = df3d::svc().inputManager().getMfiControllerListener())
        l->MFiControllerConnected();
}

- (void)controllerWasDisconnected:(NSNotification*)notification
{
    GCController *controller = (GCController *)notification.object;

    NSLog(@"%@", [NSString stringWithFormat:@"Controller disconnected:\n%@", controller.vendorName]);

    int controllerId = reinterpret_cast<uintptr_t>(controller);
    df3d::svc().inputManager().removeController(controllerId);

    if (auto l = df3d::svc().inputManager().getMfiControllerListener())
        l->MFiControllerDisconnected();

    [[UIApplication sharedApplication]setIdleTimerDisabled:NO];
}

@end

@implementation GameViewController

- (id)init
{
    self = [super init];
    g_viewController = self;
    return self;
}

- (void)dealloc
{
    [_openglView release];
    g_viewController = nil;
    [super dealloc];
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)viewDidLoad
{
    self.view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    _openglView = [[[OpenGLView alloc] initWithFrame:self.view.bounds] autorelease];
    [self.view addSubview:_openglView];

#ifndef DF3D_APPLETV
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationFade];
#else
    self.controllerUserInteractionEnabled = false;
#endif

    [super viewDidLoad];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
#ifndef DF3D_APPLETV
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationFade];
#endif
}

- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    _openglView.frame = self.view.bounds;
}


#ifdef DF3D_APPLETV

- (void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(UIPressesEvent*)event
{
    bool menuButtonHandled = false;

    for (UIPress* press in presses)
    {
        if (press.type == UIPressTypeMenu)
        {
            menuButtonHandled = df3d::HandleControllerBackButtonPressed();
        }
    }

    if (!menuButtonHandled)
    {
        self.controllerUserInteractionEnabled = true;
        [super pressesBegan:presses withEvent:event];
        self.controllerUserInteractionEnabled = false;
    }
    else
        self.controllerUserInteractionEnabled = false;
}

#endif

@end
