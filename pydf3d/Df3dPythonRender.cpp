#include "stdafx.h"

// FIXME:
// Move from here.
shared_ptr<df3d::render::Material> df3d_loadMaterial(const char *mtlLib, const char *name)
{
    return df3d::render::MaterialLib::getMaterial(mtlLib, name);
}

void bindDf3dRender()
{
    using namespace boost::python;
    using namespace df3d::render;

    def("load_material", df3d_loadMaterial);

    // Material.
    class_<Material, boost::shared_ptr<Material>, boost::noncopyable>("Material", no_init)
        .def("get_name", &Material::getName, return_value_policy<copy_const_reference>())
        .def("set_current_technique", &Material::setCurrentTechnique)
        .def("get_current_technique", &Material::getCurrentTechnique)
        .def("get_technique", &Material::getTechnique)
        .def("get_techniques_count", &Material::getTechniquesCount)
    ;

    // Technique.
    shared_ptr<RenderPass>(Technique::*getPassPy)(size_t) = &Technique::getPass;
    shared_ptr<RenderPass>(Technique::*getPassPy1)(const char *) = &Technique::getPass;
    class_<Technique, boost::shared_ptr<Technique>, boost::noncopyable>("Technique", no_init)
        .def("get_name", &Technique::getName, return_value_policy<copy_const_reference>())
        .def("get_pass", getPassPy)
        .def("get_pass", getPassPy1)
        .def("get_pass_count", &Technique::getPassCount)
    ;

    // RenderPass.
    void (RenderPass::*setDiffuseColorPy)(float, float, float, float) = &RenderPass::setDiffuseColor;
    void (RenderPass::*setDiffuseColorPy1)(const glm::vec4 &) = &RenderPass::setDiffuseColor;
    class_<RenderPass, boost::shared_ptr<RenderPass>, boost::noncopyable>("RenderPass", no_init)
        .def("set_diffuse_color", setDiffuseColorPy)
        .def("set_diffuse_color", setDiffuseColorPy1)

        // TODO:
        // Other render pass methods.
    ;
}