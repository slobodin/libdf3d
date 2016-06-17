package org.flaming0.df3d;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

public class Df3dSurfaceView extends GLSurfaceView {
    class Df3dConfigChooser implements EGLConfigChooser {
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

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
            class InnerConfig
            {
                public EGLConfig eglConfig;
                public int depthSize;
                public int r;
                public int g;
                public int b;
                public int a;
                public int stencilSize;
            };

            ArrayList<InnerConfig> bestMatches = new ArrayList<InnerConfig>();

            for (EGLConfig config : configs) {
                int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);

                // We need at least depthSize and stencilSize bits
                if (d < depthSize || s < stencilSize)
                    continue;

                // We want an exact match for red/green/blue/alpha
                int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

                if (r == redSize && g == greenSize && b == blueSize && a == alphaSize) {
                    InnerConfig cfg = new InnerConfig();
                    cfg.depthSize = d;
                    cfg.stencilSize = s;
                    cfg.r = r;
                    cfg.g = g;
                    cfg.b = b;
                    cfg.a = a;
                    cfg.eglConfig = config;
                    bestMatches.add(cfg);
                }
            }

            if (bestMatches.isEmpty()) {
                Log.e("df3d_android", "chooseConfig didn't find a EGLConfig with given parameters");
                return null;
            }

            // Now find the one with maximum depth size
            Collections.sort(bestMatches, new Comparator<InnerConfig>() {
                @Override
                public int compare(InnerConfig innerConfig, InnerConfig t1) {
                    if (innerConfig.depthSize < t1.depthSize)
                        return -1;
                    else if (innerConfig.depthSize > t1.depthSize)
                        return 1;
                    else
                        return 0;
                }
            });

            InnerConfig best = bestMatches.get(bestMatches.size() - 1);

            Log.i("df3d_android", String.format("EGLConfig: Red %d Green %d Blue %d Alpha %d Depth %d Stencil %d", best.r, best.g, best.b, best.a, best.depthSize, best.stencilSize));

            return best.eglConfig;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue) {
            if(egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }

            return defaultValue;
        }
    }


    private Df3dRenderer m_df3dRenderer = null;

    public Df3dSurfaceView(Context context) {
        super(context);

        setEGLConfigChooser(new Df3dConfigChooser(8, 8, 8, 8, 16, 8));

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
}
