#pragma once

namespace df3d {

class Any
{
    struct DataHolderBase
    {
        virtual ~DataHolderBase() = default;
        virtual DataHolderBase* clone() = 0;
    };

    template<typename T>
    struct DataHolder : DataHolderBase
    {
        T value;
        DataHolder(const T &val) : value(val) { }
        DataHolder(T &&val) : value(std::move(val)) { }

        DataHolderBase* clone() override { return new DataHolder<T>(value); }
    };

    unique_ptr<DataHolderBase> m_data;

public:
    Any() = default;
    ~Any() = default;

    template<typename T>
    Any(const T &value)
        : m_data(new DataHolder<T>(value))
    {

    }

    Any(const Any &other)
        : m_data(other.m_data ? other.m_data->clone() : nullptr)
    {

    }

    Any& operator= (Any other)
    {
        other.swap(*this);
        return *this;
    }

    void swap(Any &other)
    {
        std::swap(m_data, other.m_data);
    }

    bool empty()
    {
        return !m_data;
    }

    void clear()
    {
        m_data.reset();
    }

    template<typename T>
    const T& get() const
    {
        return (static_cast<DataHolder<T>*>(m_data.get()))->value;
    }

    template<typename T>
    T& get()
    {
        return (static_cast<DataHolder<T>*>(m_data.get()))->value;
    }
};

}
