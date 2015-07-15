package org.flaming0.df3d;

import android.content.res.AssetManager;

public class NativeBindings {
    public static native void init(int screenWidth, int screenHeight);
    public static native void draw(double dt);

    public static native void onResume();
    public static native void onPause();
    public static native void onDestroy();

    public static native void onTouchDown(int pointerId, float x, float y);
    public static native void onTouchUp(int pointerId, float x, float y);
    public static native void onTouchMove(int pointerId, float x, float y);
    public static native void onTouchCancel(int pointerId, float x, float y);

    public static native void setAssetManager(AssetManager mgr);
}
