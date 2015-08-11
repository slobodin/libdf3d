#pragma once

FWD_MODULE_CLASS(resources, DecoderMTL)
FWD_MODULE_CLASS(resources, DecoderOBJ)

namespace df3d { namespace render {

class RenderPass;
class RenderManager;

class DF3D_DLL Technique : utils::NonCopyable
{
    friend class resources::DecoderMTL;
    friend class resources::DecoderOBJ;
    friend class RenderManager;

    //! Technique id.
    std::string m_name;
    //! A list of passes. May contain passes with the same names.
    std::vector<shared_ptr<RenderPass>> m_passes;

    //! Helper. Finds first with given name.
    shared_ptr<RenderPass> findPass(const std::string &name) const;

    void appendPass(shared_ptr<RenderPass> pass);

    Technique(const std::string &name);

public:
    ~Technique();

    shared_ptr<RenderPass> getPass(int idx);
    //! Returns first pass with given name.
    shared_ptr<RenderPass> getPass(const std::string &name);
    size_t getPassCount() const;

    const std::string &getName();
};

} }
