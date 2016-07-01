#include "SparkLineTrailRenderer.h"

namespace df3d { namespace particlesys_impl {

TrailsParticleSystemRenderer::TrailsParticleSystemRenderer(size_t nbSamples, float duration, float width)
    : ParticleSystemRenderer(true),
    m_width(width),
    m_degeneratedColor(0x00000000)
{
    setNbSamples(nbSamples);
    setDuration(duration);
}

TrailsParticleSystemRenderer::~TrailsParticleSystemRenderer()
{

}

void TrailsParticleSystemRenderer::render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const
{

}

void TrailsParticleSystemRenderer::computeAABB(SPK::Vector3D &AABBMin, SPK::Vector3D &AABBMax, const SPK::Group &group, const SPK::DataSet *dataSet) const
{
    using namespace SPK;

    const Vector3D* vertexIt = SPK_GET_DATA(const Vector3DArrayData, dataSet, VERTEX_BUFFER_INDEX).getData();

    for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        ++vertexIt; // skips pre degenerated vertex
        for (size_t i = 0; i < m_nbSamples; ++i)
        {
            AABBMin.setMin(*vertexIt);
            AABBMax.setMax(*vertexIt);
            ++vertexIt;
        }
        ++vertexIt; // skips post degenerated vertex
    }
}

void TrailsParticleSystemRenderer::init(const SPK::Particle& particle, SPK::DataSet* dataSet) const
{
    using namespace SPK;

    size_t index = particle.getIndex();
    Vector3D* vertexIt = SPK_GET_DATA(Vector3DArrayData, dataSet, VERTEX_BUFFER_INDEX).getParticleData(index);
    Color* colorIt = SPK_GET_DATA(ColorArrayData, dataSet, COLOR_BUFFER_INDEX).getParticleData(index);
    float* ageIt = SPK_GET_DATA(FloatArrayData, dataSet, AGE_DATA_INDEX).getParticleData(index);
    uint8_t* startAlphaIt = SPK_GET_DATA(ArrayData<uint8_t>, dataSet, START_ALPHA_DATA_INDEX).getParticleData(index);

    // Gets the particle's values
    const Vector3D& pos = particle.position();
    const Color& color = particle.getColor();
    float age = particle.getAge();

    // Inits position
    for (size_t i = 0; i < m_nbSamples + 2; ++i)
        *(vertexIt++) = pos;

    // Inits color
    *(colorIt++) = m_degeneratedColor; // degenerate pre vertex
    for (size_t i = 0; i < m_nbSamples; ++i)
        *(colorIt++) = color;
    *colorIt = m_degeneratedColor; // degenerate post vertex

    // Inits age
    for (size_t i = 0; i < m_nbSamples; ++i)
        *(ageIt++) = age;

    // Inits start alpha
    for (size_t i = 0; i < m_nbSamples; ++i)
        *(startAlphaIt++) = color.a;
}

void TrailsParticleSystemRenderer::update(const SPK::Group& group, SPK::DataSet* dataSet) const
{
    using namespace SPK;

    Vector3D* vertexIt = SPK_GET_DATA(Vector3DArrayData, dataSet, VERTEX_BUFFER_INDEX).getData();
    Color* colorIt = SPK_GET_DATA(ColorArrayData, dataSet, COLOR_BUFFER_INDEX).getData();
    float* ageIt = SPK_GET_DATA(FloatArrayData, dataSet, AGE_DATA_INDEX).getData();
    uint8_t* startAlphaIt = SPK_GET_DATA(ArrayData<uint8_t>, dataSet, START_ALPHA_DATA_INDEX).getData();

    for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        const Particle& particle = *particleIt;
        float age = particle.getAge();

        if (age - *(ageIt + 1) >= m_duration / (m_nbSamples - 1)) // shifts the data by one
        {
            std::memmove(vertexIt + 2, vertexIt + 1, (m_nbSamples - 1) * sizeof(Vector3D));
            std::memmove(colorIt + 2, colorIt + 1, (m_nbSamples - 1) * sizeof(Color));
            std::memmove(ageIt + 1, ageIt, (m_nbSamples - 1) * sizeof(float));
            std::memmove(startAlphaIt + 1, startAlphaIt, (m_nbSamples - 1) * sizeof(uint8_t));

            // post degenerated vertex copy
            std::memcpy(vertexIt + (m_nbSamples + 1), vertexIt + m_nbSamples, sizeof(Vector3D));
        }

        // Updates the current sample
        *(vertexIt++) = particle.position();
        std::memcpy(vertexIt, vertexIt - 1, sizeof(Vector3D));
        vertexIt += m_nbSamples + 1;

        ++colorIt; // skips post degenerated vertex color
        *(colorIt++) = particle.getColor();
        *(startAlphaIt++) = particle.getColor().a;

        *(ageIt++) = age;

        // Updates alpha
        for (size_t i = 0; i < m_nbSamples - 1; ++i)
        {
            float ratio = 1.0f - (age - *(ageIt++)) / m_duration;
            (colorIt++)->a = static_cast<uint8_t>(*(startAlphaIt++) * (ratio > 0.0f ? ratio : 0.0f));
        }
        ++colorIt;
    }
}

void TrailsParticleSystemRenderer::createData(SPK::DataSet& dataSet, const SPK::Group& group) const
{
    using namespace SPK;

    dataSet.init(NB_DATA);
    dataSet.setData(VERTEX_BUFFER_INDEX, SPK_NEW(Vector3DArrayData, group.getCapacity(), m_nbSamples + 2));
    dataSet.setData(COLOR_BUFFER_INDEX, SPK_NEW(ColorArrayData, group.getCapacity(), m_nbSamples + 2));
    dataSet.setData(AGE_DATA_INDEX, SPK_NEW(FloatArrayData, group.getCapacity(), m_nbSamples));
    dataSet.setData(START_ALPHA_DATA_INDEX, SPK_NEW(ArrayData<uint8_t>, group.getCapacity(), m_nbSamples));

    // Inits the buffers
    for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        init(*particleIt, &dataSet);
}

void TrailsParticleSystemRenderer::checkData(SPK::DataSet& dataSet, const SPK::Group& group) const
{
    using namespace SPK;

    // If the number of samples has changed, we must recreate the buffers
    if (SPK_GET_DATA(FloatArrayData, &dataSet, AGE_DATA_INDEX).getSizePerParticle() != m_nbSamples)
    {
        dataSet.destroyAllData();
        createData(dataSet, group);
    }
}

} }
