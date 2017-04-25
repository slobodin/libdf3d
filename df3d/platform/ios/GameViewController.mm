//
//  GameViewController.m
//  df3d
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>
#import <AVFoundation/AVFoundation.h>
#import <GameController/GameController.h>

#import <df3d/df3d.h>
#import "AppDelegate.h"

namespace df3d {

extern bool EngineInit(EngineInitParams params);
// TODO: refactor this.
extern void AudioSuspend();
extern void AudioResume();

}

@interface GameViewController () {

}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GCController *mainController;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GameViewController {
    CGFloat screenScale;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (self.context)
    {
        GLKView *view = (GLKView *)self.view;
#ifndef DF3D_APPLETV
        view.multipleTouchEnabled = true;
#endif
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

    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(controllerWasConnected:) name:GCControllerDidConnectNotification object:nil];
    [[NSNotificationCenter defaultCenter]addObserver: self selector:@selector(controllerWasDisconnected:) name:GCControllerDidDisconnectNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan)
        {
            df3d::AudioSuspend();
        }
        else
        {
            BOOL success = [[AVAudioSession sharedInstance] setActive:TRUE error:nil];
            assert(success);

            df3d::AudioResume();
        }
    }];
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

    screenScale = [[UIScreen mainScreen] nativeScale];
    CGSize size = [[UIScreen mainScreen] nativeBounds].size;
    CGFloat screenWidth = size.width;
    CGFloat screenHeight = size.height;

#ifndef DF3D_APPLETV
    // Swap width and height for landscape orienation
	if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation))
        std::swap(screenWidth, screenHeight);
#endif

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

- (void)processTouches:(NSSet<UITouch*>*)touches withState:(df3d::Touch::State)state
{
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView: self.view];
        point.x *= screenScale;
        point.y *= screenScale;
        auto pointerId = reinterpret_cast<uintptr_t>(touch);

        df3d::svc().inputManager().onTouch(pointerId, point.x, point.y, state);
    }
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
    self.mainController = [GCController controllers][GCControllerPlayerIndex1];
    self.mainController.playerIndex = GCControllerPlayerIndex1;

    NSLog(@"%@", [NSString stringWithFormat:@"Controller connected\nName: %@\n", self.mainController.vendorName]);

    df3d::svc().inputManager().setMFiExtended(false);

    if (self.mainController.gamepad)
    {
        // A, B, X, Y
        self.mainController.gamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonA_Changed(value, pressed);
        };
        self.mainController.gamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonB_Changed(value, pressed);
        };
        self.mainController.gamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonX_Changed(value, pressed);
        };
        self.mainController.gamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_buttonY_Changed(value, pressed);
        };
        
        // Dpad
        self.mainController.gamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadLeft_Changed(value, pressed);
        };
        self.mainController.gamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadRight_Changed(value, pressed);
        };
        self.mainController.gamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadUp_Changed(value, pressed);
        };
        self.mainController.gamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_DPadDown_Changed(value, pressed);
        };
        
        // shoulder buttons
        self.mainController.gamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_LeftShoulder_Changed(value, pressed);
        };
        self.mainController.gamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_RightShoulder_Changed(value, pressed);
        };
    }
    if (self.mainController.extendedGamepad)
    {
        df3d::svc().inputManager().setMFiExtended(true);

        // two thumbsticks
        self.mainController.extendedGamepad.leftThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_LeftThumbStick_Changed(xValue, yValue);
        };
        self.mainController.extendedGamepad.rightThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_RightThumbStick_Changed(xValue, yValue);
        };

        self.mainController.extendedGamepad.leftTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_LeftTrigger_Changed(value, pressed);
        };
        self.mainController.extendedGamepad.rightTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
        {
            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                l->Mfi_RightTrigger_Changed(value, pressed);
        };
    }

    self.mainController.controllerPausedHandler = ^(GCController *controller) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener())
            l->MFiControllerPausePressed();
    };

    df3d::svc().inputManager().setHasMfiController(true);

    [[UIApplication sharedApplication]setIdleTimerDisabled:YES];

    if (auto l = df3d::svc().inputManager().getMfiControllerListener())
        l->MFiControllerConnected();
}

- (void)controllerWasDisconnected:(NSNotification*)notification
{
    GCController *controller = (GCController *)notification.object;
    NSLog(@"%@", [NSString stringWithFormat:@"Controller disconnected:\n%@", controller.vendorName]);

    df3d::svc().inputManager().setHasMfiController(false);

    if (auto l = df3d::svc().inputManager().getMfiControllerListener())
        l->MFiControllerDisconnected();
    
    self.mainController = nil;

    [[UIApplication sharedApplication]setIdleTimerDisabled:NO];
}

#ifdef DF3D_APPLETV

static bool menuButtonPressHandled = false;

- (void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(UIPressesEvent*)event
{
    menuButtonPressHandled = false;

    for (UIPress* press in presses) {
        if (press.type == UIPressTypeMenu) {
            menuButtonPressHandled = false;

            if (auto l = df3d::svc().inputManager().getMfiControllerListener())
                menuButtonPressHandled = l->MFi_menuButtonPressed();
        }
    }

    if (!menuButtonPressHandled) {
        [super pressesBegan:presses withEvent:event];
    }
}

- (void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(UIPressesEvent*)event
{
    if (!menuButtonPressHandled) {
        [super pressesEnded:presses withEvent:event];
    }
}

#endif

@end
