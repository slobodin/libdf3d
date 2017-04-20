#pragma once

#include <string>

namespace df3d {

template<typename T>
class ConfigVariable
{
    T m_val;
    std::string m_name;
    std::string m_descr;

public:
    ConfigVariable(T value, const std::string &name, const std::string &descr)
        : m_val(value),
        m_name(name),
        m_descr(descr)
    {

    }

    ConfigVariable& operator= (T val)
    {
        m_val = val;
        return *this;
    }

    ~ConfigVariable() = default;

    operator T() const
    {
        return m_val;
    }

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_descr; }
};

using ConfigVariableInt = ConfigVariable<int>;
using ConfigVariableBool = ConfigVariable<bool>;
using ConfigVariableFloat = ConfigVariable<float>;

}
