package org.flaming0.df3d;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.Log;

public class Df3dRenderer implements GLSurfaceView.Renderer {
    private static String TAG = "Df3dRenderer";

    private int m_width = 0;
    private int m_height = 0;

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(TAG, "onSurfaceCreated()");

        if (!NativeBindings.init(m_width, m_height))
            throw new RuntimeException("Failed to NativeBindings.init");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i(TAG, "onSurfaceChanged()");
        // TODO Auto-generated method stub
        //
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        NativeBindings.draw();
    }

    public void setWidthAndHeight(int w, int h) {
        m_width = w;
        m_height = h;
    }
}
