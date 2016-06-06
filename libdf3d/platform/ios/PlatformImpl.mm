#include <libdf3d/df3d.h>
#include <libdf3d/platform/Platform.h>

#import <Foundation/Foundation.h>
#import <sys/utsname.h>
#import <mach/mach.h>

namespace df3d {

size_t Platform::getProcessMemoryUsed()
{
    task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);

    kern_return_t kerr = task_info(mach_task_self(),
        TASK_BASIC_INFO,
        (task_info_t)&info,
        &size);

    if (kerr == KERN_SUCCESS)
    {
        return info.resident_size;
    }
    else
    {
        DFLOG_WARN("Error with task_info(): %s", mach_error_string(kerr));
        return 0;
    }
}

int Platform::getDPI()
{
    utsname systemInfo;
    uname(&systemInfo);

    NSString* code = [NSString stringWithCString:systemInfo.machine
                                        encoding:NSUTF8StringEncoding];

    static NSDictionary* deviceNamesByCode = nil;

    if (!deviceNamesByCode)
    {
        deviceNamesByCode = @{@"iPod1,1"        : @163,       // iPod Touch
                              @"iPod2,1"        : @163,       // iPod Touch Second Generation
                              @"iPod3,1"        : @163,       // iPod Touch Third Generation
                              @"iPod4,1"        : @326,       // iPod Touch Fourth Generation

                              @"iPhone1,1"      : @163,       // iPhone
                              @"iPhone1,2"      : @163,       // iPhone 3G
                              @"iPhone2,1"      : @163,       // iPhone 3GS

                              @"iPhone3,1"      : @326,       // iPhone 4
                              @"iPhone3,2"      : @326,       // iPhone 4
                              @"iPhone3,3"      : @326,       // iPhone 4
                              @"iPhone4,1"      : @326,       // iPhone 4S

                              @"iPhone5,1"      : @326,       // iPhone 5
                              @"iPhone5,2"      : @326,       // iPhone 5
                              @"iPhone5,3"      : @326,       // iPhone 5c
                              @"iPhone5,4"      : @326,       // iPhone 5c
                              @"iPhone6,1"      : @326,       // iPhone 5s
                              @"iPhone6,2"      : @326,       // iPhone 5s

                              @"iPhone7,1"      : @401,       // iPhone 6+
                              @"iPhone7,2"      : @326,       // iPhone 6
                              @"iPhone8,1"      : @326,       // iPhone 6S
                              @"iPhone8,2"      : @401,       // iPhone 6S+
                              @"iPhone8,4"      : @326,       // iPhone SE

                              @"iPad1,1"        : @132,       // iPad
                              @"iPad1,2"        : @132,       // iPad

                              @"iPad2,1"        : @132,       // iPad 2
                              @"iPad2,2"        : @132,       // iPad 2
                              @"iPad2,3"        : @132,       // iPad 2
                              @"iPad2,4"        : @132,       // iPad 2

                              @"iPad2,5"        : @163,       // iPad Mini
                              @"iPad2,6"        : @163,       // iPad Mini
                              @"iPad2,7"        : @163,       // iPad Mini

                              @"iPad3,1"        : @264,       // iPad 3
                              @"iPad3,2"        : @264,       // iPad 3
                              @"iPad3,3"        : @264,       // iPad 3

                              @"iPad3,4"        : @264,       // iPad 4
                              @"iPad3,5"        : @264,       // iPad 4
                              @"iPad3,6"        : @264,       // iPad 4

                              @"iPad4,1"        : @264,       // iPad Air
                              @"iPad4,2"        : @264,       // iPad Air
                              @"iPad4,3"        : @264,       // iPad Air

                              @"iPad4,4"        : @326,       // iPad Mini 2
                              @"iPad4,5"        : @326,       // iPad Mini 2
                              @"iPad4,6"        : @326,       // iPad Mini 2

                              @"iPad4,7"        : @326,       // iPad Mini 3
                              @"iPad4,8"        : @326,       // iPad Mini 3
                              @"iPad4,9"        : @326,       // iPad Mini 3

                              @"iPad5,1"        : @326,       // iPad Mini 4
                              @"iPad5,2"        : @326,       // iPad Mini 4

                              @"iPad5,3"        : @264,       // iPad Air 2
                              @"iPad5,4"        : @264,       // iPad Air 2

                              @"iPad6,3"        : @264,       // iPad Pro
                              @"iPad6,4"        : @264,       // iPad Pro
                              @"iPad6,7"        : @264,       // iPad Pro
                              @"iPad6,8"        : @264,       // iPad Pro
                              };

    }

    NSInteger deviceDPIValue = [[deviceNamesByCode objectForKey:code] integerValue];
    if (!deviceDPIValue)
        deviceDPIValue = 264; // default 264 for future device.

    return deviceDPIValue;
}

}
