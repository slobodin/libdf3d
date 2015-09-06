#pragma once

#include <render/RenderPass.h>

namespace df3d { namespace render {

class DF3D_DLL Technique
{
    //! Technique id.
    std::string m_name;
    //! A list of passes. May contain passes with the same names.
    std::vector<RenderPass> m_passes;

    //! Helper. Finds first with given name.
    RenderPass* findPass(const std::string &name);

public:
    Technique(const std::string &name);
    ~Technique();

    void appendPass(const RenderPass &pass);

    //! Returns ith pass.
    RenderPass* getPass(int idx);
    //! Returns first pass with given name.
    RenderPass* getPass(const std::string &name);
    size_t getPassCount() const;

    const std::string& getName() const;
};

} }
