#include "JsonUtils.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <rapidjson/reader.h>

namespace df3d {

namespace JsonUtils {

rapidjson::Document fromFile(const char *path)
{
    if (auto fileSource = svc().resourceManager().getFS().open(path))
    {
        auto retVal = fromFile(*fileSource);
        if (retVal.IsNull())
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

rapidjson::Document fromFile(ResourceDataSource &dataSource)
{
    std::string buffer;
    buffer.resize(dataSource.getSize());
    dataSource.read(&buffer[0], buffer.size());
    return fromString(buffer);
}

rapidjson::Document fromString(const std::string &data)
{
    rapidjson::Document d;
    if (d.Parse<rapidjson::kParseCommentsFlag>(data.c_str()).HasParseError())
    {
        DFLOG_WARN("Failed to parse json. Error: %s", "TODO_serial");
        return{};
    }

    return d;
}

}

}
