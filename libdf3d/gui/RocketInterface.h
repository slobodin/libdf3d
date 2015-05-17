#pragma once

#include <Rocket/Core/String.h>
#include <Rocket/Core/FileInterface.h>
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/RenderInterface.h>

#include <render/RenderOperation.h>

FWD_MODULE_CLASS(resources, FileDataSource)
FWD_MODULE_CLASS(render, GpuProgram)
FWD_MODULE_CLASS(render, Renderer)
FWD_MODULE_CLASS(render, RenderPass)
FWD_MODULE_CLASS(render, Texture)

namespace df3d { namespace gui {

//! Interface to Rocket GUI library.
class GuiFileInterface : public Rocket::Core::FileInterface
{
    std::unordered_map<Rocket::Core::FileHandle, shared_ptr<resources::FileDataSource>> m_openedFiles;
public:
    GuiFileInterface();
    ~GuiFileInterface();

    //! Opens a file.
    Rocket::Core::FileHandle Open(const Rocket::Core::String& path);

    //! Closes a previously opened file.
    void Close(Rocket::Core::FileHandle file);

    //! Reads data from a previously opened file.
    size_t Read(void* buffer, size_t size, Rocket::Core::FileHandle file);

    //! Seeks to a point in a previously opened file.
    bool Seek(Rocket::Core::FileHandle file, long offset, int origin);

    //! Returns the current position of the file pointer.
    size_t Tell(Rocket::Core::FileHandle file);
};

class GuiSystemInterface : public Rocket::Core::SystemInterface
{
    TimePoint m_appStarted;
public:
    GuiSystemInterface();

    //! Get the number of seconds elapsed since the start of the application
    /** @returns Seconds elapsed
      */
    float GetElapsedTime();

    bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String &message);
};

class GuiRenderInterface : public Rocket::Core::RenderInterface
{
    //! Shader for 2d rendering.
    shared_ptr<render::GpuProgram> m_ffp2d;
    //! Gui rendering pass properties.
    shared_ptr<render::RenderPass> m_guipass;
    //! Gui render operation holder.
    unique_ptr<render::RenderOperation> m_renderOperation;
    
    std::unordered_map<uint32_t, shared_ptr<render::Texture>> m_textures;
    uint32_t m_textureId;
public:
    GuiRenderInterface();

    void SetViewport(int width, int height);

    //! Called by Rocket when it wants to render geometry that it does not wish to optimize.
    void RenderGeometry(Rocket::Core::Vertex *vertices, int num_vertices, 
        int *indices, int num_indices, 
        Rocket::Core::TextureHandle texture, 
        const Rocket::Core::Vector2f &translation);

    //! Called by Rocket when it wants to compile geometry it believes will be static for the foreseeable future.
    Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex *vertices, int num_vertices, 
        int *indices, int num_indices, 
        Rocket::Core::TextureHandle texture);

    //! Called by Rocket when it wants to render application-compiled geometry.
    void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, 
        const Rocket::Core::Vector2f &translation);
    //! Called by Rocket when it wants to release application-compiled geometry.
    void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

    //! Called by Rocket when it wants to enable or disable scissoring to clip content.
    void EnableScissorRegion(bool enable);
    //! Called by Rocket when it wants to change the scissor region.
    void SetScissorRegion(int x, int y, int width, int height);

    //! Called by Rocket when a texture is required by the library.
    bool LoadTexture(Rocket::Core::TextureHandle &texture_handle, Rocket::Core::Vector2i &texture_dimensions, 
        const Rocket::Core::String &source);
    //! Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
    bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, 
        const Rocket::Core::byte *source, const Rocket::Core::Vector2i &source_dimensions);
    //! Called by Rocket when a loaded texture is no longer required.
    void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);
}; 

} }
