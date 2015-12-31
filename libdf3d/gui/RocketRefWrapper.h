#pragma once

#include <Rocket/Core.h>

namespace df3d {

template<typename T>
class RocketRefWrapper
{
    T *m_ref = nullptr;

public:
    RocketRefWrapper() = default;
    RocketRefWrapper(T *ref)
        : m_ref(ref)
    {
        if (m_ref)
            m_ref->AddReference();
    }

    RocketRefWrapper(const RocketRefWrapper &other)
        : m_ref(other.m_ref)
    {
        if (m_ref)
            m_ref->AddReference();
    }

    template<typename U>
    RocketRefWrapper(const RocketRefWrapper<U> &other)
        : m_ref(other.m_ref)
    {
        if (m_ref)
            m_ref->AddReference();
    }

    ~RocketRefWrapper()
    {
        if (m_ref)
            m_ref->RemoveReference();
    }

    RocketRefWrapper& operator= (RocketRefWrapper other)
    {
        other.swap(*this);
        return *this;
    }

    template<typename U>
    RocketRefWrapper& operator= (RocketRefWrapper<U> other)
    {
        other.swap(*this);
        return *this;
    }

    void reset()
    {
        RocketRefWrapper().swap(*this);
    }

    T& operator* () const
    {
        return *m_ref;
    }

    T* operator-> () const
    {
        return m_ref;
    }

    void swap(RocketRefWrapper &other)
    {
        std::swap(m_ref, other.m_ref);
    }
};

using RocketDocument = RocketRefWrapper<Rocket::Core::ElementDocument>;
using RocketElement = RocketRefWrapper<Rocket::Core::Element>;

}
