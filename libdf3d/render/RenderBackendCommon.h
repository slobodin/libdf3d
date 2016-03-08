#pragma once

namespace df3d {

using Descriptor = int;

using VertexBufferDescriptor = Descriptor;
using IndexBufferDescriptor = Descriptor;
using TextureDescriptor = Descriptor;
using ShaderDescriptor = Descriptor;
using GpuProgramDescriptor = Descriptor;

class DescriptorPool
{
public:
    Descriptor alloc() { static int i; return i++; }
    void free(Descriptor d) { }
};

}
