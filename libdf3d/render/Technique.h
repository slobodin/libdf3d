#pragma once

namespace df3d {

class RenderPass;

class DF3D_DLL Technique
{
    //! Technique id.
    std::string m_name;
    //! A list of passes. May contain passes with the same names.
    std::vector<shared_ptr<RenderPass>> m_passes;

    //! Helper. Finds first with given name.
    shared_ptr<RenderPass> findPass(const std::string &name);

public:
    Technique(const std::string &name);
    Technique(const Technique &other);
    Technique& operator= (Technique other);
    ~Technique();

    void appendPass(const RenderPass &pass);

    //! Returns ith pass.
    shared_ptr<RenderPass> getPass(int idx);
    //! Returns first pass with given name.
    shared_ptr<RenderPass> getPass(const std::string &name);
    //! Returns passes count.
    size_t getPassCount() const;

    const std::string& getName() const;
};

}
