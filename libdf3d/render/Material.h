#pragma once

FWD_MODULE_CLASS(resources, DecoderMTL)
FWD_MODULE_CLASS(resources, DecoderOBJ)

namespace df3d { namespace render {

class Technique;
class RenderManager;

//! Material is a collection of rendering techniques.
class DF3D_DLL Material : boost::noncopyable
{
    friend class resources::DecoderMTL;
    friend class resources::DecoderOBJ;
    friend class RenderManager;

    //! Material name.
    std::string m_name;
    //! A list of techniques.
    std::vector<shared_ptr<Technique>> m_techniques;

    //! Current technique being used by this material.
    shared_ptr<Technique> m_currentTechnique;

    //! Helper function.
    shared_ptr<Technique> findTechnique(const std::string &name) const;

    void appendTechnique(shared_ptr<Technique> technique);

    Material(const std::string &name);

public:
    ~Material();

    const std::string &getName() const;

    void setCurrentTechnique(const char *name);
    shared_ptr<Technique> getCurrentTechnique();

    shared_ptr<Technique> getTechnique(const char *name);
    size_t getTechniquesCount() const;
};

} }