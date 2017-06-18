package org.flaming0.df3d;

import android.app.Activity;
import android.os.Build;
import android.os.Debug;
import android.util.Log;

public class Df3dAndroidServices {
    private Df3dLocalStorage storage = null;

    private long stringToInt(String s)
    {
        if (s != null)
            return Long.valueOf(s);
        return 0;
    }

    public Df3dAndroidServices(Activity activity) {
        storage = new Df3dLocalStorage(activity);
    }

    public Object getLocalStorage() { return storage; }

    public void exitApp(/*int exitCode*/) {
        System.exit(0);
    }

    public long getProcessMemUsage() {
        if (Build.VERSION.SDK_INT >= 23) {
            Debug.MemoryInfo memInfo = new Debug.MemoryInfo();
            Debug.getMemoryInfo(memInfo);

            String javaHeap = memInfo.getMemoryStat("summary.java-heap");
            String nativeHeap = memInfo.getMemoryStat("summary.native-heap");
            String code = memInfo.getMemoryStat("summary.code");
            String stack = memInfo.getMemoryStat("summary.stack");
            String privateOther = memInfo.getMemoryStat("summary.private-other");
            String system = memInfo.getMemoryStat("summary.system");

            long resultKb = stringToInt(javaHeap) + stringToInt(nativeHeap) +
                    stringToInt(code) + stringToInt(stack) +
                    stringToInt(privateOther) + stringToInt(system);

            return resultKb << 10;
        }

        return 0;
    }

    public long getGraphicsMemUsage() {
        if (Build.VERSION.SDK_INT >= 23) {
            Debug.MemoryInfo memInfo = new Debug.MemoryInfo();
            Debug.getMemoryInfo(memInfo);

            String graphics = memInfo.getMemoryStat("summary.graphics");

            long resultKb = stringToInt(graphics);

            return resultKb << 10;
        }

        return 0;
    }
}
