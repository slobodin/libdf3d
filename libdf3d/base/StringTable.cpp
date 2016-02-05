#include "StringTable.h"

#include <libdf3d/utils/Utils.h>
#include <libdf3d/utils/JsonUtils.h>

namespace df3d {

StringTable::StringTable(const std::string &path)
{
    auto jsonData = utils::json::fromFile(path);

    for (auto kv = jsonData.begin(); kv != jsonData.end(); kv++)
        m_table[kv.key().asString()] = kv->asString();
}

StringTable::~StringTable()
{

}

const std::string& StringTable::translateString(const std::string &id) const
{
    auto found = m_table.find(id);
    if (found != m_table.end())
        return found->second;
    return m_missingEntry;
}

bool StringTable::hasTranslation(const std::string &id) const
{
    return utils::contains_key(m_table, id);
}

}
