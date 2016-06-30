#pragma once

namespace df3d {

class Technique;

//! Material is a collection of rendering techniques.
class DF3D_DLL Material
{
    //! Material name.
    std::string m_name;
    //! A list of techniques.
    std::vector<shared_ptr<Technique>> m_techniques;

    //! Current technique being used by this material.
    shared_ptr<Technique> m_currentTechnique = nullptr;

    //! Helper function.
    shared_ptr<Technique> findTechnique(const std::string &name);

public:
    Material(const std::string &name = "");
    Material(const Material &other);
    Material& operator= (Material other);
    ~Material();

    const std::string &getName() const;

    void appendTechnique(const Technique &technique);

    void setCurrentTechnique(const std::string &name);
    shared_ptr<Technique> getCurrentTechnique();

    shared_ptr<Technique> getTechnique(const std::string &name);
    size_t getTechniquesCount() const;
};

}
