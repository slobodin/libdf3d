#include <df3d/platform/LocalNotification.h>
#include <df3d/platform/android/JNIHelpers.h>

namespace df3d {

bool LocalNotification::schedule(int notificationID, const char *message, double secondsFromNow)
{
    auto env = AndroidServices::getEnv();
    auto jservices = AndroidServices::getServices();

    jclass cls = env->GetObjectClass(jservices);
    jmethodID methId = env->GetMethodID(cls, "scheduleLocalNotification", "(Ljava/lang/String;II)V");

    jstring jmessage = env->NewStringUTF(message);
    env->CallVoidMethod(jservices, methId, jmessage, (int)secondsFromNow, notificationID);

    env->DeleteLocalRef(jmessage);
    env->DeleteLocalRef(cls);

    return true;
}

void LocalNotification::cancel(int notificationID)
{
    auto env = AndroidServices::getEnv();
    auto jservices = AndroidServices::getServices();

    jclass cls = env->GetObjectClass(jservices);
    jmethodID methId = env->GetMethodID(cls, "cancelLocalNotification", "(I)V");

    env->CallVoidMethod(jservices, methId, notificationID);

    env->DeleteLocalRef(cls);
}

void LocalNotification::registerLocalNotifications()
{

}

}
