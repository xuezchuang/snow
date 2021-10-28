#include "gpu_attr_binding.h"
#include "GPUShader.h"

void AttrBinding_clear(GPUAttrBinding* binding)
{
    binding->loc_bits = 0;
    binding->enabled_bits = 0;
}

uint read_attr_location(const GPUAttrBinding* binding, uint a_idx)
{
    assert(a_idx < GPU_VERT_ATTR_MAX_LEN);
    assert(binding->enabled_bits & (1 << a_idx));
    return (binding->loc_bits >> (4 * a_idx)) & 0xF;
}

static void write_attr_location(GPUAttrBinding* binding, uint a_idx, uint location)
{
    assert(a_idx < GPU_VERT_ATTR_MAX_LEN);
    assert(location < GPU_VERT_ATTR_MAX_LEN);
    const uint shift = 4 * a_idx;
    const uint64_t mask = ((uint64_t)0xF) << shift;
    /* overwrite this attr's previous location */
    binding->loc_bits = (binding->loc_bits & ~mask) | (location << shift);
    /* mark this attr as enabled */
    binding->enabled_bits |= 1 << a_idx;
}

uint GPU_shaderinterface_attr(GLuint program,const char* name)
{
	assert(program != 0);
	return glGetAttribLocation(program, name);
}

void get_attr_locations(const GPUVertFormat* format, GPUAttrBinding* binding)
{
    AttrBinding_clear(binding);

    for (uint a_idx = 0; a_idx < format->attr_len; a_idx++) {
        const GPUVertAttr* a = &format->attrs[a_idx];
        for (uint n_idx = 0; n_idx < a->name_len; n_idx++) {
            const char* name = GPU_vertformat_attr_name_get(format, a, n_idx);
            uint location = GPU_shaderinterface_attr(1,name);
            write_attr_location(binding, a_idx, location);
        }
    }
}
