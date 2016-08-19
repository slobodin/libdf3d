#pragma once

#include <ctime>

namespace df3d {

class LocalNotification
{
public:
    static bool schedule(int notificationID, const char *msg, std::tm timePoint);
    static void cancel(int notificationID);
    static void enableNotifications(bool enable);
};

}
