#include "df3d_pch.h"
#include "../Storage.h"

#include <utils/JsonHelpers.h>

namespace df3d { namespace platform { 

class WindowsStorage : public Storage
{
public:
    WindowsStorage(const char *filename)
        : Storage(filename)
    {
        auto root = utils::jsonLoadFromFile(filename);
        if (!!root)
        {
            df3d::base::glog << "Failed to init Storage. Can not open file" << df3d::base::logwarn;
            return;
        }

        for (auto it = root.begin(); it != root.end(); it++)
        {
            const auto key = it.key().asString();
            Storage::Entry val;

            if (it->isBool())
                val = it->asBool();
            else if (it->isInt())
                val = it->isInt();
            else if (it->isInt64())
                val = it->isInt64();
            else if (it->isDouble())
                val = it->asDouble();
            else if (it->isString())
                val = it->asString();

            m_entries[key] = val;
        }
    }

    virtual void save() override
    {

    }
};

Storage *Storage::create(const char *filename)
{
    return new WindowsStorage(filename);
}

} }
