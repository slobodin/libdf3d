#include <df3d/df3d.h>
#include "PlatformStorage.h"
#import <Foundation/Foundation.h>

namespace df3d {

bool PlatformStorage::saveData(const char *id, const PodArray<uint8_t> &data)
{
    NSData *storageData = [NSData dataWithBytes:data.data() length:data.size()];

    [[NSUserDefaults standardUserDefaults] setObject:storageData forKey:[NSString stringWithUTF8String:id]];
    return [[NSUserDefaults standardUserDefaults] synchronize];
}

void PlatformStorage::getData(const char *id, PodArray<uint8_t> &data)
{
    data.clear();

    NSData *storageData = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithUTF8String:id]];
    if (storageData == nil)
        return;

    const uint8_t *bytes = reinterpret_cast<const uint8_t*>([storageData bytes]);
    data.assign(bytes, [storageData length]);
}

}
