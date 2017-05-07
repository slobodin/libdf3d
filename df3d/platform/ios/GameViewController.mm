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
#ifndef DF3D_APPLETV
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView: self.view];
        point.x *= screenScale;
        point.y *= screenScale;
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

- (BOOL)handleControllerBackButtonPressed:(BOOL)pressed
{
    if (auto l = df3d::svc().inputManager().getMfiControllerListener())
        return l->Mfi_buttonBack_Pressed(pressed);

    return false;
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
#ifndef DF3D_APPLETV
        controller.gamepad.buttonB.pressedChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (pressed)
                [self handleControllerBackButtonPressed:pressed];
        };
#endif
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

#ifdef DF3D_APPLETV

static bool menuButtonPressHandled = false;

- (void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(UIPressesEvent*)event
{
    menuButtonPressHandled = false;

    for (UIPress* press in presses)
    {
        if (press.type == UIPressTypeMenu)
        {
            menuButtonPressHandled = [self handleControllerBackButtonPressed:true];
        }
    }

    if (!menuButtonPressHandled)
    {
        [super pressesBegan:presses withEvent:event];
    }
}

#endif

@end
