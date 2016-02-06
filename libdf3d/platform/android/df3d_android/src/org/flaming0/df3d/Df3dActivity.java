package org.flaming0.df3d;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.content.res.AssetManager;

public class Df3dActivity extends Activity {
    private Df3dSurfaceView m_glSurfaceView = null;
    private AssetManager m_assetManager = null;

    static
    {
        System.loadLibrary("openal");
        System.loadLibrary("RocketCore");
        System.loadLibrary("RocketControls");
        System.loadLibrary("RocketDebugger");

        // TODO: load client library from meta-data.
        System.loadLibrary("ships3d");

        Log.i("df3d_android", "df3d native libraries loaded");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        m_assetManager = getAssets();
        NativeBindings.setAssetManager(m_assetManager);

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
}
