package org.flaming0.df3d;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;

public class Df3dPreferences {
    private SharedPreferences prefs = null;

    public Df3dPreferences(Activity activity) {
        this.prefs = activity.getPreferences(Context.MODE_PRIVATE);
    }

    public void setString(String key, String value) {
        SharedPreferences.Editor editor = prefs.edit();
        editor.putString(key, value);

        editor.commit();
    }

    public String getString(String key) {
        String defaultVal = "";
        return prefs.getString(key, defaultVal);
    }
}
