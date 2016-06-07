package org.flaming0.df3d;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.content.res.AssetManager;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

public class Df3dActivity extends Activity {
    private Df3dSurfaceView m_glSurfaceView = null;
    private AssetManager m_assetManager = null;
    private Df3dAndroidServices services = null;
    private boolean hasFocus = false;

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

        final Window window = getWindow();

        if (window != null)
            window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onPause() {
        super.onPause();

        m_glSurfaceView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();

        resumeIfHasFocus();
    }

    @Override
    protected void onDestroy() {
        // Won't call on destroy in native code.
        // NativeBindings.onDestroy();
        super.onDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);

        this.hasFocus = hasFocus;

        if (hasFocus)
            enableFullscreen();

        resumeIfHasFocus();
    }

    private void resumeIfHasFocus() {
        if (this.hasFocus)
            m_glSurfaceView.onResume();
    }

    private void enableFullscreen()
    {
        if (Build.VERSION.SDK_INT >= 16) {
            int uiOptions = getWindow().getDecorView().getSystemUiVisibility();

            uiOptions |= View.SYSTEM_UI_FLAG_FULLSCREEN;

            if (Build.VERSION.SDK_INT >= 19) {
                uiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
                uiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
                uiOptions |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
                uiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
                uiOptions |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
            }

            getWindow().getDecorView().setSystemUiVisibility(uiOptions);
        } else {
            requestWindowFeature(Window.FEATURE_NO_TITLE);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }
    }
}
