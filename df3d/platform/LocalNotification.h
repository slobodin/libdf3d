#pragma once

namespace df3d {

class LocalNotification
{
public:
    using NotificationId = int;
    static const NotificationId InvalidId = -1;

    static NotificationId schedule(const char *msg);
    static void cancel(NotificationId notificationId);
    static void enableNotifications(bool enable);
};

}
