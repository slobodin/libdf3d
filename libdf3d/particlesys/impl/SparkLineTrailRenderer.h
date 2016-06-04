#pragma once

#include "SparkCommon.h"
#include <libdf3d/render/Vertex.h>

namespace df3d {

namespace particlesys_impl {

class TrailsParticleSystemRenderer : public ParticleSystemRenderer
{
    spark_description(TrailsParticleSystemRenderer, ParticleSystemRenderer)

private:
    struct SpkVertexData
    {
        SPK::Vector3D pos;
        float tx_u;
        float tx_v;
        glm::vec4 color;
    };

    size_t m_nbSamples;
    float m_width;
    float m_duration;
    SPK::Color m_degeneratedColor;

    static const size_t NB_DATA = 4;
    static const size_t VERTEX_BUFFER_INDEX = 0;
    static const size_t COLOR_BUFFER_INDEX = 1;
    static const size_t AGE_DATA_INDEX = 2;
    static const size_t START_ALPHA_DATA_INDEX = 3;

    TrailsParticleSystemRenderer(size_t nbSamples = 8, float duration = 1.0f, float width = 1.0f);

public:
    ~TrailsParticleSystemRenderer();

    /**
    * @brief Sets the number of samples in a trail
    *
    * The number of samples defines the number of points used to construct the trail.<br>
    * The bigger the number of samples, the smoother the trail but the bigger the compution time and the memory consumption.
    *
    * @param nbSamples : the number of samples to construct the trails
    */
    void setNbSamples(size_t nbSamples)
    {
        DF3D_ASSERT(nbSamples >= 2, "GLLineTrailRenderer::setNbSamples(size_t) - The number of samples cannot be less than 2");
        m_nbSamples = nbSamples;
    }

    /**
    * @brief Gets the number of samples per trail
    * @return the number of samples per trail
    */
    size_t getNbSamples() const
    {
        return m_nbSamples;
    }

    /**
    * @brief Sets the duration of a sample
    *
    * The duration of a sample is defined by its life time from its creation to its destruction.<br>
    * Note that the alpha of a sample will decrease linearly from its initial alpha to 0.
    *
    * @param duration : the duration of a sample
    */
    void setDuration(float duration)
    {
        DF3D_ASSERT(m_nbSamples > 0.0f, "GLLineTrailRenderer::setDuration(float) - The duration cannot be less or equal to 0.0f");
        m_duration = duration;
    }

    /**
    * @brief Gets the duration of a sample
    * @return the duration of a sample
    */
    float getDuration() const
    {
        return m_duration;
    }

    /**
    * @brief Sets the width of a trail
    *
    * Like for GLLineRenderer, the width is defined in pixels and is not dependant of the distance of the trail from the camera
    *
    * @param width : the width of trails in pixels
    */
    void setWidth(float width)
    {
        m_width = width;
    }

    /**
    * @brief Gets the width of a trail
    * @return the width of a trail (in pixels)
    */
    float getWidth() const
    {
        return m_width;
    }

    /**
    * @brief Sets the color components of degenerated lines
    * @param color : the color of the degenerated lines
    */
    void setDegeneratedLines(SPK::Color color)
    {
        m_degeneratedColor = color;
    }

    void render(const SPK::Group &group, const SPK::DataSet *dataSet, SPK::RenderBuffer *renderBuffer) const override;
    void computeAABB(SPK::Vector3D &AABBMin, SPK::Vector3D &AABBMax, const SPK::Group &group, const SPK::DataSet *dataSet) const override;
    void init(const SPK::Particle& particle, SPK::DataSet* dataSet) const override;
    void update(const SPK::Group& group, SPK::DataSet* dataSet) const override;
    void createData(SPK::DataSet& dataSet, const SPK::Group& group) const override;
    void checkData(SPK::DataSet& dataSet, const SPK::Group& group) const override;

    // Creates and registers a new TrailsParticleSystemRenderer.
    static SPK::Ref<TrailsParticleSystemRenderer> create(size_t nbSamples = 8, float duration = 1.0f, float width = 1.0f)
    {
        return SPK_NEW(TrailsParticleSystemRenderer, nbSamples, duration, width);
    }
};

} }
