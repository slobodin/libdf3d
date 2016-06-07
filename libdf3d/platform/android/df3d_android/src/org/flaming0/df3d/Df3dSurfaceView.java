package org.flaming0.df3d;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

public class Df3dSurfaceView extends GLSurfaceView {
    class Df3dConfigChooser implements EGLConfigChooser {
        final private static String TAG = "Df3dConfigChooser";
        final private static int EGL_OPENGL_ES2_BIT = 4;

        private int[] mValue = new int[1];
        protected int redSize;
        protected int greenSize;
        protected int blueSize;
        protected int alphaSize;
        protected int depthSize;
        protected int stencilSize;

        public Df3dConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            redSize = r;
            greenSize = g;
            blueSize = b;
            alphaSize = a;
            depthSize = depth;
            stencilSize = stencil;
        }

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            // Our minimum requirements for the graphics context
            int[] minimumSpec = {
                // We want OpenGL ES 2
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                // We want to render to a window
                EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
                // We do not want a translucent window, otherwise the
                // home screen or activity in the background may shine through
                EGL10.EGL_TRANSPARENT_TYPE, EGL10.EGL_NONE,
                // indicate that this list ends:
                EGL10.EGL_NONE
            };

            int[] arg = new int[1];
            egl.eglChooseConfig(display, minimumSpec, null, 0, arg);
            int numConfigs = arg[0];

            if (numConfigs <= 0) {
                // The minimum spec is not available here
                return null;
            }

            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, minimumSpec, configs, numConfigs, arg);

            EGLConfig chosen = chooseConfig(egl, display, configs);
            if (chosen == null) {
                throw new RuntimeException("Failed to choose EGL config");
            }

            return chosen;
        }

        /**
         * This method iterates through the list of configurations that
         * fulfill our minimum requirements and tries to pick one that matches best
         * our requested color, depth and stencil buffer requirements that were set using
         * the constructor of this class.
         */
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                                      EGLConfig[] configs) {
            EGLConfig bestMatch = null;
            int bestR = Integer.MAX_VALUE, bestG = Integer.MAX_VALUE,
                    bestB = Integer.MAX_VALUE, bestA = Integer.MAX_VALUE,
                    bestD = Integer.MAX_VALUE, bestS = Integer.MAX_VALUE;

            for(EGLConfig config : configs) {
                int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config,
                        EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config,
                        EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config,
                        EGL10.EGL_ALPHA_SIZE, 0);
                int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);

                if(r <= bestR && g <= bestG && b <= bestB && a <= bestA
                        && d <= bestD && s <= bestS && r >= redSize
                        && g >= greenSize && b >= blueSize
                        && a >= alphaSize && d >= depthSize
                        && s >= stencilSize) {
                    bestR = r;
                    bestG = g;
                    bestB = b;
                    bestA = a;
                    bestD = d;
                    bestS = s;
                    bestMatch = config;
                }
            }

            return bestMatch;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                                     EGLConfig config, int attribute, int defaultValue) {

            if(egl.eglGetConfigAttrib(display, config, attribute,
                    mValue)) {
                return mValue[0];
            }

            return defaultValue;
        }
    }


    private Df3dRenderer m_df3dRenderer = null;

    public Df3dSurfaceView(Context context) {
        super(context);

        setEGLConfigChooser(new Df3dConfigChooser(8, 8, 8, 8, 24, 8));

        try {
            setPreserveEGLContextOnPause(true);
        }
        catch(Exception e) {
            e.printStackTrace();
        }

        setEGLContextClientVersion(2);
        setFocusableInTouchMode(true);

        setRenderer(m_df3dRenderer = new Df3dRenderer());
    }

    @Override
    public void onResume() {
        super.onResume();
//        setRenderMode(RENDERMODE_CONTINUOUSLY);

        Log.i("df3d_android", "onResume");

        queueEvent(new Runnable() {
            @Override
            public void run() {
                NativeBindings.onResume();
            }
        });
    }

    @Override
    public void onPause() {
        super.onPause();

        Log.i("df3d_android", "onPause");

        queueEvent(new Runnable() {
            @Override
            public void run() {
                NativeBindings.onPause();
            }
        });

//        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                NativeBindings.onSurfaceDestroyed();
            }
        });

        super.surfaceDestroyed(holder);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        super.onTouchEvent(event);

        final int pointerCount = event.getPointerCount();
        final int[] ids = new int[pointerCount];
        final float[] xCoords = new float[pointerCount];
        final float[] yCoords = new float[pointerCount];

        for (int i = 0; i < pointerCount; i++) {
            ids[i] = event.getPointerId(i);
            xCoords[i] = event.getX(i);
            yCoords[i] = event.getY(i);
        }

        final int p = event.getActionIndex();
        final float x = event.getX(p);
        final float y = event.getY(p);
        final int pointerId = event.getPointerId(p);

        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        NativeBindings.onTouchDown(pointerId, x, y);
                    }
                });
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        NativeBindings.onTouchUp(pointerId, x, y);
                    }
                });
                break;
            case MotionEvent.ACTION_MOVE:
                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        for (int i = 0; i < pointerCount; i++)
                            NativeBindings.onTouchMove(ids[i], xCoords[i], yCoords[i]);
                    }
                });
                break;
            case MotionEvent.ACTION_CANCEL:
                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        for (int i = 0; i < pointerCount; i++)
                            NativeBindings.onTouchCancel(ids[i], xCoords[i], yCoords[i]);
                    }
                });
                break;
            default:
                break;
        };

        return true;
    }

    @Override
    public void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        m_df3dRenderer.setWidthAndHeight(w, h);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        super.onKeyDown(keyCode, event);

        // TODO:
        // Process key down.

        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        super.onKeyUp(keyCode, event);

        // TODO:
        // Process key up.

        return true;
    }
}
