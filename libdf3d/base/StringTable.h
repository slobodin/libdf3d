#pragma once

namespace df3d {

class DF3D_DLL StringTable : utils::NonCopyable
{
    std::string m_missingEntry = "missing_entry";
    std::unordered_map<std::string, std::string> m_table;

public:
    StringTable(const std::string &path);
    ~StringTable();

    const std::string& translateString(const std::string &id) const;
    bool hasTranslation(const std::string &id) const;
};

}
