//
//  GameViewController.m
//  df3d
//

#import "GameViewController.h"
#import <AVFoundation/AVFoundation.h>
#import <GameController/GameController.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <df3d/df3d.h>
#import "AppDelegate.h"

static GameViewController* g_viewController;

namespace df3d {
    
bool IOSDeviceOrientationIsLandscapeLeft()
{
#ifndef DF3D_APPLETV
    return [[UIDevice currentDevice] orientation] == UIDeviceOrientationLandscapeLeft;
#endif
    return false;
}

extern bool EngineInit(EngineInitParams params);
// TODO: refactor this.
extern void AudioSuspend();
extern void AudioResume();

extern void SetAccelerationValuesIOS(float x, float y, float z);
extern void SetAccelerometerSupportedIOS(bool);

bool HandleControllerBackButtonPressed()
{
    if (auto l = df3d::svc().inputManager().getMfiControllerListener())
        return l->Mfi_buttonMenu_Pressed();

    return false;
}

#ifndef DF3D_APPLETV
void StartAccelerometerListenerIOS()
{
    [g_viewController startAccelerometerListener];
}

void StopAccelerometerListenerIOS()
{
    [g_viewController stopAccelerometerListener];
}
#endif

}

@implementation DF3DView {
    int m_viewportX;
    int m_viewportY;
    
    float m_screenScale;
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size
{
    m_viewportX = size.width;
    m_viewportY = size.height;
}

- (void)drawInMTKView:(MTKView *)view
{
    df3d::svc().step();
}

- (bool) startupEngine
{
    CGSize screenSize = [[UIScreen mainScreen] nativeBounds].size;
    
#ifndef DF3D_APPLETV
    if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation))
        std::swap(screenSize.width, screenSize.height);
#endif

    assert(df3d::AppDelegate::getInstance() != nullptr);

    auto engineInitParams = df3d::AppDelegate::getInstance()->getInitParams();
    engineInitParams.windowWidth = screenSize.width;
    engineInitParams.windowHeight = screenSize.height;
    engineInitParams.hardwareData = self;

    if (!df3d::EngineInit(engineInitParams)) {
        DFLOG_CRITICAL("Failed to startup engine!");
        return false;
    }

    if (!df3d::AppDelegate::getInstance()->onAppStarted()) {
        DFLOG_CRITICAL("Game code initialization failed");
        return false;
    }

    AVAudioSession *session = [AVAudioSession sharedInstance];
    BOOL success = FALSE;
    success = [session setCategory:AVAudioSessionCategoryAmbient error:nil];
    success = [session setActive:TRUE error:nil];

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

    self.delegate = self;

    [self mtkView:self drawableSizeWillChange:self.drawableSize];

    return true;
}

- (instancetype) initWithFrame:(CGRect)frame device:(id<MTLDevice>)device
{
    if (!device) {
        NSLog(@"Failed to init View. Metal device is not available");
        return nil;
    }
    
    m_screenScale = [[UIScreen mainScreen] nativeScale];

    self = [super initWithFrame:frame device:device];

    if (self) {
        m_viewportX = 0;
        m_viewportY = 0;

#ifndef DF3D_APPLETV
        self.multipleTouchEnabled = true;
#endif
        [self setPreferredFramesPerSecond:60.0];
        [self setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [self setDepthStencilPixelFormat:MTLPixelFormatDepth32Float];
        [self setUserInteractionEnabled:YES];
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [super dealloc];
}

- (void)processTouches:(NSSet<UITouch*>*)touches withState:(df3d::Touch::State)state
{
#ifndef DF3D_APPLETV
    for (UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView: self];
        point.x *= m_screenScale;
        point.y *= m_screenScale;
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
{
    DF3DView *m_metalView;
}

- (id)init
{
    self = [super init];

    m_metalView = nil;
    g_viewController = self;

#ifndef DF3D_APPLETV
    self.motionManager = [[CMMotionManager alloc] init];
    self.motionQueue = [[NSOperationQueue alloc] init];

    self.motionManager.accelerometerUpdateInterval = 1.0f / 60.0f;
    df3d::SetAccelerometerSupportedIOS([self.motionManager isAccelerometerAvailable]);

    self.listeningAccelerometer = false;
#endif

    return self;
}

- (void)dealloc
{
    [m_metalView release];

    g_viewController = nil;

#ifndef DF3D_APPLETV
    [self.motionQueue release];
    [self.motionManager release];
#endif

    [super dealloc];
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

#ifndef DF3D_APPLETV

- (void) startAccelerometerListener
{
    if (self.listeningAccelerometer)
        return;

    self.listeningAccelerometer = true;

    [self.motionManager startAccelerometerUpdatesToQueue:self.motionQueue withHandler:
     ^(CMAccelerometerData *accelerometerData, NSError *error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            float x = accelerometerData.acceleration.x;
            float y = accelerometerData.acceleration.y;
            float z = accelerometerData.acceleration.z;

            df3d::SetAccelerationValuesIOS(x, y, z);
        });
     }];
}

- (void) stopAccelerometerListener
{
    if (!self.listeningAccelerometer)
        return;

    [self.motionManager stopAccelerometerUpdates];

    self.listeningAccelerometer = false;
}

#endif

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    m_metalView = [[DF3DView alloc] initWithFrame:[[UIScreen mainScreen] bounds] device:MTLCreateSystemDefaultDevice()];
    [self.view addSubview:m_metalView];

#ifndef DF3D_APPLETV
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationFade];
#else
    self.controllerUserInteractionEnabled = false;
#endif

    [m_metalView startupEngine];

#ifndef DF3D_APPLETV
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
#endif
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
    m_metalView.frame = self.view.bounds;
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
