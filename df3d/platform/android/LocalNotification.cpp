#include <df3d/platform/LocalNotification.h>

namespace df3d {

bool LocalNotification::schedule(int notificationID, const char *msg, double secondsFromNow)
{
    DFLOG_WARN("Local notifcations for Android is not implemented yet.");
    return false;
}

void LocalNotification::cancel(int notificationID)
{

}

void LocalNotification::registerLocalNotifications()
{

}

}
