//
//  GLView.h
//  df3d
//

#import "GLView.h"

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import <libdf3d/df3d.h>

@implementation GLView {
    CAEAGLLayer *eaglLayer;
    EAGLContext *eaglContext;
    CADisplayLink *displayLink;

    GLuint framebuffer;
    GLuint colorBuffer;
    GLuint depthBuffer;
}

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    [self setMultipleTouchEnabled:YES];
    [self setContentScaleFactor:[UIScreen mainScreen].scale];

    eaglLayer = (CAEAGLLayer *)self.layer;
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = @{
                                     kEAGLDrawablePropertyRetainedBacking : @NO,
                                     kEAGLDrawablePropertyColorFormat : kEAGLColorFormatRGBA8,
                                     };

    eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:eaglContext];

    displayLink = [[CADisplayLink displayLinkWithTarget:self selector:@selector(mainLoop:)] retain];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

    CGSize size = self.bounds.size;
    size.width *= self.contentScaleFactor;
    size.height *= self.contentScaleFactor;

    [self createRenderbuffers:size];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillResignActive)
                                                 name:UIApplicationWillResignActiveNotification
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationDidBecomeActive)
                                                 name:UIApplicationDidBecomeActiveNotification
                                               object:nil];

    return self;
}

- (void) dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [EAGLContext setCurrentContext:eaglContext];
    [self destroyRenderbuffers];

    [displayLink invalidate];
    [displayLink release];

    [EAGLContext setCurrentContext:nil];
    [eaglContext release];

    [super dealloc];
}

- (void)mainLoop:(id)sender
{
    [EAGLContext setCurrentContext:eaglContext];

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    df3d::svc().step();

    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)createRenderbuffers:(CGSize)size
{
    int width = int(size.width);
    int height = int(size.height);

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenRenderbuffers(1, &colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLenum error;
    if ((error = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
        NSLog(@"Failed to make complete framebuffer!");
}

- (void)destroyRenderbuffers
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (colorBuffer)
    {
        glDeleteRenderbuffers(1, &colorBuffer);
        colorBuffer = 0;
    }

    if (depthBuffer)
    {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }

    if (framebuffer)
    {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
}

- (void)applicationWillResignActive
{
    glFinish();

    [EAGLContext setCurrentContext:eaglContext];
    [self destroyRenderbuffers];

    [displayLink invalidate];
    [displayLink release];
    displayLink = nil;

    [EAGLContext setCurrentContext:nil];
}

- (void)applicationDidBecomeActive
{
    if (!displayLink)
    {
        CGSize size = self.bounds.size;
        size.width *= self.contentScaleFactor;
        size.height *= self.contentScaleFactor;
        [EAGLContext setCurrentContext:eaglContext];
        [self createRenderbuffers:size];

        displayLink = [[CADisplayLink displayLinkWithTarget:self selector:@selector(mainLoop:)] retain];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
}

@end
