#include <libdf3d/platform/LocalNotification.h>

namespace df3d {

LocalNotification::NotificationId LocalNotification::schedule(const char *msg)
{
    return InvalidId;
}

void LocalNotification::cancel(NotificationId notificationId)
{

}

void LocalNotification::enableNotifications(bool enable)
{

}

}
