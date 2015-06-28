package org.flaming0.df3d;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class Df3dSurfaceView extends GLSurfaceView {
	private Df3dRenderer m_df3dRenderer = null;

    public Df3dSurfaceView(Context context) {
        super(context);

        setEGLContextClientVersion(2);
        setFocusableInTouchMode(true);

        setRenderer(m_df3dRenderer = new Df3dRenderer());
    }

    @Override
    public void onResume() {
        super.onResume();
        setRenderMode(RENDERMODE_CONTINUOUSLY);

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

        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        super.onTouchEvent(event);

		switch (event.getAction() & MotionEvent.ACTION_MASK) {
			case MotionEvent.ACTION_DOWN:
				final float xDown = event.getX(0);
				final float yDown = event.getY(0);

				queueEvent(new Runnable() {
					@Override
					public void run() {
						NativeBindings.onTouchDown(0, xDown, yDown);
					}
				});
				break;
			case MotionEvent.ACTION_UP:
				final float xUp = event.getX(0);
				final float yUp = event.getY(0);

				queueEvent(new Runnable() {
					@Override
					public void run() {
						NativeBindings.onTouchUp(0, xUp, yUp);
					}
				});
				break;
			case MotionEvent.ACTION_MOVE:
				int pointerCount = event.getPointerCount();
				for (int i = 0; i < pointerCount; i++) {
					final float xMove = event.getX(i);
					final float yMove = event.getY(i);

					queueEvent(new Runnable() {
						@Override
						public void run() {
							NativeBindings.onTouchMove(0, xMove, yMove);
						}
					});
				}
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
