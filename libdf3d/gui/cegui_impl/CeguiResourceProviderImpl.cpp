#include "df3d_pch.h"
#include "CeguiResourceProviderImpl.h"

#include <CEGUI/CEGUI.h>

#include <base/Controller.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>

namespace df3d  { namespace gui { namespace cegui_impl {

using namespace CEGUI;

CeguiResourceProviderImpl::CeguiResourceProviderImpl()
{

}

void CeguiResourceProviderImpl::loadRawDataContainer(const CEGUI::String &filename, CEGUI::RawDataContainer &output, const CEGUI::String &resourceGroup)
{
    auto fileSource = g_fileSystem->openFile(filename.c_str());
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
    return 0;
}

} } }