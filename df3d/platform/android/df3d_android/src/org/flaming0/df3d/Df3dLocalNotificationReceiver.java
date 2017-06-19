package org.flaming0.df3d;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

public class Df3dLocalNotificationReceiver extends BroadcastReceiver {
    boolean isAppRunning() {
        return false;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i("df3d_android", "Df3dLocalNotificationReceiver::onReceive");

        if(isAppRunning())
            return; // Suppress notifications when activity in foreground

        try {
            int id = intent.getIntExtra("id", 0);
            String message = intent.getStringExtra("message");

            Notification notification = makeNotification(context, message);
            NotificationManager manager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
            manager.notify(id, notification);
        } catch (Exception e) {
            e.printStackTrace();
            Log.e("df3d_android", "Failed to make local notification!");
        }
    }

    private Intent createNotificationIntent(Context context)
    {
        final PackageManager pm = context.getPackageManager();
        return pm.getLaunchIntentForPackage(context.getPackageName());
    }

    private String getAppName(Context context) {
        try {
            ApplicationInfo appInfo = context.getApplicationInfo();
            return (String)context.getPackageManager().getApplicationLabel(appInfo);
        } catch (Exception e) {
            e.printStackTrace();
            return "empty";
        }
    }

    private Notification makeNotification(Context context, String message) {
        ApplicationInfo appInfo = context.getApplicationInfo();

        Intent intent = createNotificationIntent(context);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        int appIconId = appInfo.icon;
        Bitmap largeIcon = BitmapFactory.decodeResource(context.getResources(), appIconId);

        Notification.Builder builder = new Notification.Builder(context.getApplicationContext());

        builder.setContentTitle(getAppName(context));
        builder.setContentText(message);
        builder.setTicker(message);
        builder.setSmallIcon(appIconId);
        builder.setLargeIcon(largeIcon);
        builder.setAutoCancel(true);
        builder.setDefaults(Notification.DEFAULT_ALL);
        builder.setContentIntent(pendingIntent);

        return builder.build();
    }
}
