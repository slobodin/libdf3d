#include <libdf3d/platform/LocalNotification.h>

namespace df3d {

LocalNotification::NotificationId LocalNotification::schedule(const char *msg)
{
    DFLOG_DEBUG("LocalNotification::schedule is unavailable for desktop platform");
    return InvalidId;
}

void LocalNotification::cancel(NotificationId notificationId)
{
    DFLOG_DEBUG("LocalNotification::cancel is unavailable for desktop platform");
}

void LocalNotification::enableNotifications(bool enable)
{
    DFLOG_DEBUG("LocalNotification::enableNotifications is unavailable for desktop platform");
}

}
