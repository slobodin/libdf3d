package org.flaming0.df3d;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;

public class Df3dRenderer implements GLSurfaceView.Renderer {
    private int m_width = 0;
    private int m_height = 0;
    private long m_prevTime = 0;

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        // TODO:
        // Context recreate also here.

        NativeBindings.init(m_width, m_height);
        m_prevTime = System.nanoTime();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        // TODO Auto-generated method stub
        //
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        long currTime = System.nanoTime();
        long dt = currTime - m_prevTime;

        m_prevTime = currTime;

        NativeBindings.draw((double)dt / 1000000000.0);
    }

    public void setWidthAndHeight(int w, int h) {
        m_width = w;
        m_height = h;
    }
}
