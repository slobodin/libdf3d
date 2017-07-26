package org.flaming0.df3d;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

public class Df3dSurfaceView extends GLSurfaceView {
    private static String TAG = "Df3dSurfaceView";

     private class Df3dConfigChooser implements EGLConfigChooser {
        final private static int EGL_OPENGL_ES2_BIT = 4;

        private int[] m_value;

        Df3dConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            m_value = new int[] {r, g, b, a, depth, stencil};
        }

        private EGLConfig doChooseConfig(EGL10 egl, EGLDisplay display, int[] attributes) {
            EGLConfig[] configs = new EGLConfig[1];
            int[] matchedConfigNum = new int[1];
            boolean result = egl.eglChooseConfig(display, attributes, configs, 1, matchedConfigNum);
            if (result && matchedConfigNum[0] > 0) {
                return configs[0];
            }
            return null;
        }

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            int[][] EGLAttributes = {
                    {
                            // GL ES 2 with user set
                            EGL10.EGL_RED_SIZE, m_value[0],
                            EGL10.EGL_GREEN_SIZE, m_value[1],
                            EGL10.EGL_BLUE_SIZE, m_value[2],
                            EGL10.EGL_ALPHA_SIZE, m_value[3],
                            EGL10.EGL_DEPTH_SIZE, m_value[4],
                            EGL10.EGL_STENCIL_SIZE, m_value[5],
                            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                            EGL10.EGL_NONE
                    },
                    {
                            // GL ES 2 by default
                            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                            EGL10.EGL_NONE
                    }
            };

            EGLConfig result;
            for (int[] eglAtribute : EGLAttributes) {
                result = this.doChooseConfig(egl, display, eglAtribute);
                if (result != null)
                    return result;
            }

            MyLog.e(TAG, "Can not select an EGLConfig for rendering.");
            return null;
        }
    }

    private Df3dRenderer m_df3dRenderer = null;

    public Df3dSurfaceView(Context context) {
        super(context);

        getHolder().setFormat(PixelFormat.TRANSLUCENT);

        setEGLConfigChooser(new Df3dConfigChooser(8, 8, 8, 8, 16, 8));

        try {
            setPreserveEGLContextOnPause(true);
        }
        catch(Exception e) {
            MyLog.e(TAG, "Failed to setPreserveEGLContextOnPause(true)");
            MyLog.e(TAG, e.getMessage());
        }

        setEGLContextClientVersion(2);
        setFocusableInTouchMode(true);

        setRenderer(m_df3dRenderer = new Df3dRenderer((Df3dActivity)context));
    }

    @Override
    public void onResume() {
        super.onResume();

        MyLog.i(TAG, "onResume");

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

        MyLog.i(TAG, "onPause");

        queueEvent(new Runnable() {
            @Override
            public void run() {
                NativeBindings.onPause();
            }
        });
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
