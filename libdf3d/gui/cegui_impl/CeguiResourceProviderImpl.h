#pragma once

#include <CEGUI/ResourceProvider.h>

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiResourceProviderImpl : public CEGUI::ResourceProvider
{
    std::map<CEGUI::String, CEGUI::String, CEGUI::StringFastLessCompare> m_resourceGroups;

public:
    CeguiResourceProviderImpl();

    void setResourceGroupDirectory(const CEGUI::String &resourceGroup, const CEGUI::String &directory);

    void loadRawDataContainer(const CEGUI::String &filename, CEGUI::RawDataContainer &output, const CEGUI::String &resourceGroup);
    void unloadRawDataContainer(CEGUI::RawDataContainer &output);
    size_t getResourceGroupFileNames(std::vector<CEGUI::String> &out_vec, const CEGUI::String &file_pattern, const CEGUI::String &resource_group);

    CEGUI::String getFinalFilename(const CEGUI::String &filename, const CEGUI::String &resourceGroup);
};

} } }