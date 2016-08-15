#pragma once

namespace df3d {

class LocalNotification
{
public:
    using NotificationId = int;

    static NotificationId schedule(const char *msg);
    static void cancel(NotificationId notificationId);
    static void enableNotifications(bool enable);
};

}
