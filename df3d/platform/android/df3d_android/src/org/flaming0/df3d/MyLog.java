package org.flaming0.df3d;

import android.util.Log;

public class MyLog {
    public static boolean LOGGING_ENABLED = false;

    public static boolean enabled() {
        return LOGGING_ENABLED;
    }

    public static void d(String tag, String message) {
        if (LOGGING_ENABLED) {
            Log.d(tag, message);
        }
    }
    public static void i(String tag, String message) {
        if (LOGGING_ENABLED) {
            Log.i(tag, message);
        }
    }
    public static void v(String tag, String message) {
        if (LOGGING_ENABLED) {
            Log.v(tag, message);
        }
    }
    public static void w(String tag, String message) {
        if (LOGGING_ENABLED) {
            Log.w(tag, message);
        }
    }
    public static void e(String tag, String message) {
        if (LOGGING_ENABLED) {
            Log.e(tag, message);
        }
    }
}
