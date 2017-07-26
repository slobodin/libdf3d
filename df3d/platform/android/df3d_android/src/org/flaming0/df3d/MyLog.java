package org.flaming0.df3d;

import android.util.Log;

public class MyLog {
    private static final boolean LOGGING = false;

    public static void d(String tag, String message) {
        if (LOGGING) {
            Log.d(tag, message);
        }
    }
    public static void i(String tag, String message) {
        if (LOGGING) {
            Log.i(tag, message);
        }
    }
    public static void v(String tag, String message) {
        if (LOGGING) {
            Log.v(tag, message);
        }
    }
    public static void w(String tag, String message) {
        if (LOGGING) {
            Log.w(tag, message);
        }
    }
    public static void e(String tag, String message) {
        if (LOGGING) {
            Log.e(tag, message);
        }
    }
}
