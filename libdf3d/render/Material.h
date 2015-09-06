#pragma once

#include "Technique.h"

namespace df3d { namespace render {

//! Material is a collection of rendering techniques.
class DF3D_DLL Material
{
    //! Material name.
    std::string m_name;
    //! A list of techniques.
    std::vector<Technique> m_techniques;

    //! Current technique being used by this material.
    Technique *m_currentTechnique = nullptr;

    //! Helper function.
    Technique* findTechnique(const std::string &name);

public:
    Material(const std::string &name = "");
    ~Material();

    const std::string &getName() const;

    void appendTechnique(const Technique &technique);

    void setCurrentTechnique(const std::string &name);
    Technique* getCurrentTechnique();

    Technique* getTechnique(const std::string &name);
    size_t getTechniquesCount() const;
};

} }
