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

		final int pointerCount = event.getPointerCount();
		final int[] ids = new int[pointerCount];
		final float[] xCoords = new float[pointerCount];
		final float[] yCoords = new float[pointerCount];

		for (int i = 0; i < pointerCount; i++) {
			ids[i] = event.getPointerId(i);
			xCoords[i] = event.getX(i);
			yCoords[i] = event.getY(i);
		}

		switch (event.getActionMasked()) {
			case MotionEvent.ACTION_DOWN:
				queueEvent(new Runnable() {
					@Override
					public void run() {
						NativeBindings.onTouchDown(ids[0], xCoords[0], yCoords[0]);
					}
				});
				break;
			case MotionEvent.ACTION_UP:
				queueEvent(new Runnable() {
					@Override
					public void run() {
						NativeBindings.onTouchUp(ids[0], xCoords[0], yCoords[0]);
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
			case MotionEvent.ACTION_POINTER_UP:
				final int idxUp = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
				final int idUp = event.getPointerId(idxUp);
				final float xUp = event.getX(idxUp);
				final float yUp = event.getY(idxUp);

				queueEvent(new Runnable() {
					@Override
					public void run() {
						NativeBindings.onTouchUp(idUp, xUp, yUp);
					}
				});
				break;
			case MotionEvent.ACTION_POINTER_DOWN:
				final int idxDown = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
				final int idDown = event.getPointerId(idxDown);
				final float xDown = event.getX(idxDown);
				final float yDown = event.getY(idxDown);

				queueEvent(new Runnable() {
					@Override
					public void run() {
						NativeBindings.onTouchDown(idDown, xDown, yDown);
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
