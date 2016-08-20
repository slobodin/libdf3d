#pragma once

namespace df3d {

class LocalNotification
{
public:
    static bool schedule(int notificationID, const char *msg, double secondsFromNow);
    static void cancel(int notificationID);
    static void registerLocalNotifications();
};

}
