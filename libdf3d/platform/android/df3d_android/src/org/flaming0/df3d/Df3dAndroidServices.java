package org.flaming0.df3d;

import android.app.Activity;

public class Df3dAndroidServices {
    private Df3dPreferences prefs = null;
    private Df3dLocalStorage storage = null;

    public Df3dAndroidServices(Activity activity) {
        prefs = new Df3dPreferences(activity);
        storage = new Df3dLocalStorage(activity);
    }

    public Df3dPreferences getPreferences() {
        return prefs;
    }

    public Object getLocalStorage() { return storage; }
}
