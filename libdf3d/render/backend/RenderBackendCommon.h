#pragma once

namespace df3d {

using Descriptor = int;

using VertexBuffer2 = Descriptor;
using IndexBuffer2 = Descriptor;
using Texture2 = Descriptor;
using Shader2 = Descriptor;
using GpuProgram2 = Descriptor;

class DescriptorPool
{
public:
    Descriptor alloc() { static int i; return i++; }
    void free(Descriptor d) { }
};

}
