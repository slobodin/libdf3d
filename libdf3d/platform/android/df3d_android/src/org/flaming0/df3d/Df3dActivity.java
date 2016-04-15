package org.flaming0.df3d;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.content.res.AssetManager;
import android.view.View;

public class Df3dActivity extends Activity {
    private Df3dSurfaceView m_glSurfaceView = null;
    private AssetManager m_assetManager = null;
    private Df3dAndroidServices services = null;

    static
    {
        System.loadLibrary("openal");

        // TODO: load client library from meta-data.
        System.loadLibrary("ships3d");

        Log.i("df3d_android", "df3d native libraries loaded");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        enableFullscreen();

        services = new Df3dAndroidServices(this);
        NativeBindings.servicesInitialized(services);
        NativeBindings.setAssetManager(m_assetManager = getAssets());

        setContentView(m_glSurfaceView = new Df3dSurfaceView(this));
    }

    @Override
    protected void onPause() {
        super.onPause();

        m_glSurfaceView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();

        m_glSurfaceView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        NativeBindings.onDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus)
            enableFullscreen();
    }

    private void enableFullscreen()
    {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_FULLSCREEN |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }
}
