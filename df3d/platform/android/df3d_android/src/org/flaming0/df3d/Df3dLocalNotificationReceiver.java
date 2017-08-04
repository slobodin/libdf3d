package org.flaming0.df3d;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;

public class Df3dLocalNotificationReceiver extends BroadcastReceiver {
    private static String TAG = "Df3dLocalNotifRecv";

    boolean isAppRunning() {
        return Df3dActivity.getSharedActivity() != null &&
                Df3dActivity.getSharedActivity().isAppRunning();
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        MyLog.i(TAG, "onReceive()");

        if(isAppRunning())
            return; // Suppress notifications when activity in foreground

        try {
            int id = intent.getIntExtra("id", 0);
            String message = intent.getStringExtra("message");

            Notification notification = makeNotification(context, message);
            NotificationManager manager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
            manager.notify(id, notification);
        } catch (Exception e) {
            MyLog.e(TAG, "Failed to make local notification!");
            MyLog.e(TAG, e.getMessage());
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
            MyLog.e(TAG, e.getMessage());
            return "empty";
        }
    }

    private Notification makeNotification(Context context, String message) {
        ApplicationInfo appInfo = context.getApplicationInfo();

        Intent intent = createNotificationIntent(context);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

//        Bitmap largeIcon = BitmapFactory.decodeResource(context.getResources(), appIconId);

        int smallIcon = context.getResources().getIdentifier("ic_notification", "mipmap", context.getPackageName());
        if(smallIcon == 0)
            smallIcon = appInfo.icon;

        Notification.Builder builder = new Notification.Builder(context.getApplicationContext());

        builder.setContentTitle(getAppName(context));
        builder.setContentText(message);
        builder.setTicker(message);
        builder.setSmallIcon(smallIcon);
//        builder.setLargeIcon(largeIcon);
        builder.setAutoCancel(true);
        builder.setDefaults(Notification.DEFAULT_ALL);
        builder.setContentIntent(pendingIntent);

        return builder.build();
    }
}
