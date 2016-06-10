#include <libdf3d/df3d.h>
#include <libdf3d/io/Storage.h>

#import <Foundation/Foundation.h>

namespace df3d { namespace platform_impl {

class IOSStorage : public Storage
{
    void saveToFileSystem(const uint8_t *data, size_t size) override
    {
        NSData *storageData = [NSData dataWithBytes:data length:size];

        [[NSUserDefaults standardUserDefaults] setObject:storageData forKey:@"df3ds"];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    bool getFromFileSystem(uint8_t **data, size_t *size) override
    {
        NSData *storageData = [[NSUserDefaults standardUserDefaults] objectForKey:@"df3ds"];
        if (storageData == nil) {
            *size = 0;
            return false;
        }

        size_t len = [storageData length];

        *data = new uint8_t[len];
        memcpy(*data, [storageData bytes], len);

        *size = len;

        return true;
    }

public:
    IOSStorage(const std::string &filename)
        : Storage(filename)
    {

    }
};

}

Storage* Storage::create(const std::string &filename)
{
    return new platform_impl::IOSStorage(filename);
}

}
