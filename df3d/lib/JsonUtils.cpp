#include "JsonUtils.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceDataSource.h>

namespace df3d {

namespace JsonUtils {

Json::Value fromFile(const char *path)
{
    if (auto fileSource = svc().resourceManager().getFS().open(path))
    {
        auto retVal = fromFile(*fileSource);
        if (retVal.isNull())
            DFLOG_WARN("Failed to parse json from %s", path);

        svc().resourceManager().getFS().close(fileSource);

        return retVal;
    }
    else
    {
        DFLOG_WARN("Couldn't load json configs from %s. File not found", path);
        return{};
    }
}

Json::Value fromFile(ResourceDataSource &dataSource)
{
    std::string buffer;
    buffer.resize(dataSource.getSize());
    dataSource.read(&buffer[0], buffer.size());
    return fromString(buffer);
}

Json::Value fromString(const std::string &data)
{
    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(data, root))
    {
        DFLOG_WARN("Failed to parse json. Error: %s", reader.getFormattedErrorMessages().c_str());
        return{};
    }

    return root;
}

}

}
