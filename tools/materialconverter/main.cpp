#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <boost/algorithm/string.hpp>

struct MaterialProps
{
    std::string name;

    double shininess = 0.0;
    double ambient[3];
    double diffuse[3];
    double specular[3];
    double emissive[3];

    std::string map_Kd;
    std::string map_Ks;

    MaterialProps()
        : ambient{ }, diffuse{ }, specular{ }, emissive{ }
    {

    }
};

MaterialProps *currentMaterial = nullptr;

std::vector<MaterialProps> loadMaterials(std::istream &is)
{
    std::string tok;
    std::vector<MaterialProps> materials;
    while (is >> tok)
    {
        boost::trim(tok);
        if (tok.empty() || tok.at(0) == '#')
        {
            // Skip line.
            std::string temp;
            std::getline(is, temp);
            continue;
        }

        if (tok == "newmtl")
        {
            MaterialProps newmtl;
            is >> newmtl.name;

            if (newmtl.name.empty())
                throw std::runtime_error("Found material with empty name.");

            materials.push_back(newmtl);
            currentMaterial = &materials.back();
        }
        else if (tok == "Ns")
        {
            is >> currentMaterial->shininess;
        }
        else if (tok == "Ka")
        {
            is >> currentMaterial->ambient[0] >> currentMaterial->ambient[1] >> currentMaterial->ambient[2];
        }
        else if (tok == "Kd")
        {
            is >> currentMaterial->diffuse[0] >> currentMaterial->diffuse[1] >> currentMaterial->diffuse[2];
        }
        else if (tok == "Ks")
        {
            is >> currentMaterial->specular[0] >> currentMaterial->specular[1] >> currentMaterial->specular[2];
        }
        else if (tok == "Ke")
        {
            is >> currentMaterial->emissive[0] >> currentMaterial->emissive[1] >> currentMaterial->emissive[2];
        }
        else if (tok == "map_Kd")
        {
            is >> currentMaterial->map_Kd;
        }
        else if (tok == "map_Ks")
        {
            is >> currentMaterial->map_Ks;
        }
    }

    return materials;
}

void saveMaterials(std::ostream &os, const std::vector<MaterialProps> &materials)
{
    if (!os)
        throw std::exception("Can not write output.");

    // FIXME:
    // Reduce boilerplate.
    std::string indent = "            ";

    auto samplerWriter = [&](const std::string &name, const std::string &path)
    {
        if (path.empty())
            return;

        os << "\n";
        os << indent << "sampler " << name << " {\n";
        os << indent << "    " << "path " << path << "\n";
        os << indent << "    type TEXTURE_2D\n";
        os << indent << "    filtering TRILINEAR\n";
        os << indent << "    wrap_mode CLAMP\n";
        os << indent << "    mipmaps true\n";
        os << indent << "}\n";
    };

    for (const auto &material : materials)
    {
        os << "material " << material.name << " {\n";
        os << "    technique " << "tech_1" << " {\n";
        os << "        pass {\n";

        os << indent << "ambient " << material.ambient[0] << " " << material.ambient[1] << " " << material.ambient[2] << "\n";
        os << indent << "diffuse " << material.diffuse[0] << " " << material.diffuse[1] << " " << material.diffuse[2] << "\n";
        os << indent << "specular " << material.specular[0] << " " << material.specular[1] << " " << material.specular[2] << "\n";
        os << indent << "emissive " << material.emissive[0] << " " << material.emissive[1] << " " << material.emissive[2] << "\n";

        // Write defaults.
        os << indent << "polygon_mode FILL" << "\n";
        os << indent << "front_face CCW" << "\n";
        os << indent << "cull_face BACK" << "\n";
        os << indent << "depth_test true" << "\n";
        os << indent << "is_lit true" << "\n";

        samplerWriter("diffuseMap", material.map_Kd);
        samplerWriter("specularMap", material.map_Ks);

        os << "        }\n";
        os << "    }\n";
        os << "}\n\n";
    }
}

int main(int argc, const char **argv) try
{
    if (argc > 3 || argc <= 1)
    {
        throw std::runtime_error("Invalid input. Usage: materialconverter.exe material.mtl [output.mtl]");
    }

    std::string outputFileName;
    if (argc == 3)
    {
        outputFileName = argv[2];
    }

    std::ifstream file(argv[1]);
    if (!file)
    {
        throw std::runtime_error("Can not open file " + std::string(argv[1]));
    }

    auto materials = loadMaterials(file);
    file.close();

    if (materials.empty())
    {
        std::cerr << "None of materials was collected.\n";
        return 0;
    }

    // Do save.
    std::ofstream out;
    if (outputFileName.empty())
        out.open(argv[1]);
    else
        out.open(outputFileName);

    saveMaterials(out, materials);

    std::cerr << "Done! " << materials.size() << " materials was converted.\n";

    return 0;
}
catch (std::exception &e)
{
    std::cerr << "An error occurred:\n" << e.what() << "\n";
    return 1;
}