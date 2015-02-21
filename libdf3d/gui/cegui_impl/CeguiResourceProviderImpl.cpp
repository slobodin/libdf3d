#include "df3d_pch.h"
#include "CeguiResourceProviderImpl.h"

#include <CEGUI/CEGUI.h>

#include <base/SystemsMacro.h>
#include <resources/FileDataSource.h>

namespace df3d  { namespace gui { namespace cegui_impl {

using namespace CEGUI;

CeguiResourceProviderImpl::CeguiResourceProviderImpl()
{

}

void CeguiResourceProviderImpl::setResourceGroupDirectory(const CEGUI::String &resourceGroup, const CEGUI::String &directory)
{
    if (directory.length() == 0)
        return;

    const CEGUI::String separators("\\/");

    if (separators.find(directory[directory.length() - 1]) == CEGUI::String::npos)
        m_resourceGroups[resourceGroup] = directory + '/';
    else
        m_resourceGroups[resourceGroup] = directory;
}

void CeguiResourceProviderImpl::loadRawDataContainer(const CEGUI::String &filename, CEGUI::RawDataContainer &output, const CEGUI::String &resourceGroup)
{
    auto fileSource = g_fileSystem->openFile(getFinalFilename(filename, resourceGroup).c_str());
    if (!fileSource)
        CEGUI_THROW(InvalidRequestException("Unable to open resource file '" + filename + "'."));

    const auto bufSize = fileSource->getSize();
    unsigned char *buf = new unsigned char[bufSize];

    fileSource->getRaw(buf, bufSize);

    output.setData(buf);
    output.setSize(bufSize);
}

void CeguiResourceProviderImpl::unloadRawDataContainer(CEGUI::RawDataContainer &output)
{
    if (output.getDataPtr())
    {
        delete [] output.getDataPtr();
        output.setData(0);
        output.setSize(0);
    }
}

size_t CeguiResourceProviderImpl::getResourceGroupFileNames(std::vector<CEGUI::String> &out_vec, const CEGUI::String &file_pattern, const CEGUI::String &resource_group)
{
    // TODO:
    CEGUI_THROW(InvalidRequestException("getResourceGroupFileNames for libdf3d is not implemented."));
    return 0;
}

CEGUI::String CeguiResourceProviderImpl::getFinalFilename(const CEGUI::String &filename, const CEGUI::String &resourceGroup)
{
    String finalFilename;

    auto iter = m_resourceGroups.find(resourceGroup.empty() ? d_defaultResourceGroup : resourceGroup);
    if (iter != m_resourceGroups.end())
        finalFilename = (*iter).second;

    return finalFilename + filename;
}

} } }
