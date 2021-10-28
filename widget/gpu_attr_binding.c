#include "GPU_vertex_format.h"
#include <string.h>
#include <assert.h>
void GPU_vertformat_clear(GPUVertFormat* format)
{
	memset(format, 0, sizeof(GPUVertFormat));
}

void GPU_vertformat_copy(GPUVertFormat* dest, const GPUVertFormat* src)
{
	/* copy regular struct fields */
	memcpy(dest, src, sizeof(GPUVertFormat));
}
static uchar copy_attr_name(GPUVertFormat* format, const char* name)
{
	/* strncpy does 110% of what we need; let's do exactly 100% */
	uchar name_offset = format->name_offset;
	char* name_copy = format->names + name_offset;
	uint available = GPU_VERT_ATTR_NAMES_BUF_LEN - name_offset;
	bool terminated = false;

	for (uint i = 0; i < available; i++) {
		const char c = name[i];
		name_copy[i] = c;
		if (c == '\0') {
			terminated = true;
			format->name_offset += (i + 1);
			break;
		}
	}
	assert(terminated);
	assert(format->name_offset <= GPU_VERT_ATTR_NAMES_BUF_LEN);
	return name_offset;
}

static GLenum convert_comp_type_to_gl(GPUVertCompType type)
{
	static const GLenum table[] = {
		[GPU_COMP_I8] = GL_BYTE,
		[GPU_COMP_U8] = GL_UNSIGNED_BYTE,
		[GPU_COMP_I16] = GL_SHORT,
		[GPU_COMP_U16] = GL_UNSIGNED_SHORT,
		[GPU_COMP_I32] = GL_INT,
		[GPU_COMP_U32] = GL_UNSIGNED_INT,

		[GPU_COMP_F32] = GL_FLOAT,

		[GPU_COMP_I10] = GL_INT_2_10_10_10_REV,
	};
	return table[type];
}


static uint comp_sz(GPUVertCompType type)
{
	assert(type <= GPU_COMP_F32); /* other types have irregular sizes (not bytes) */
	const GLubyte sizes[] = { 1, 1, 2, 2, 4, 4, 4 };
	return sizes[type];
}

static uint attr_sz(const GPUVertAttr* a)
{
	if (a->comp_type == GPU_COMP_I10) {
		return 4; /* always packed as 10_10_10_2 */
	}
	return a->comp_len * comp_sz(a->comp_type);
}

uint padding(uint offset, uint alignment)
{
	const uint mod = offset % alignment;
	return (mod == 0) ? 0 : (alignment - mod);
}


uint GPU_vertformat_attr_add(GPUVertFormat* format,const char* name,GPUVertCompType comp_type,uint comp_len,GPUVertFetchMode fetch_mode)
{
    assert(format->name_len < GPU_VERT_FORMAT_MAX_NAMES); /* there's room for more */
    assert(format->attr_len < GPU_VERT_ATTR_MAX_LEN);     /* there's room for more */
    assert(!format->packed);                              /* packed means frozen/locked */
    assert((comp_len >= 1 && comp_len <= 4) || comp_len == 8 || comp_len == 12 || comp_len == 16);

    switch (comp_type) {
    case GPU_COMP_F32:
        /* float type can only kept as float */
        assert(fetch_mode == GPU_FETCH_FLOAT);
        break;
    case GPU_COMP_I10:
        /* 10_10_10 format intended for normals (xyz) or colors (rgb)
         * extra component packed.w can be manually set to { -2, -1, 0, 1 } */
        assert(comp_len == 3 || comp_len == 4);

        /* Not strictly required, may relax later. */
        assert(fetch_mode == GPU_FETCH_INT_TO_FLOAT_UNIT);

        break;
    default:
        /* integer types can be kept as int or converted/normalized to float */
        assert(fetch_mode != GPU_FETCH_FLOAT);
        /* only support float matrices (see Batch_update_program_bindings) */
        assert(comp_len != 8 && comp_len != 12 && comp_len != 16);
    }
    format->name_len++; /* multiname support */

    const uint attr_id = format->attr_len++;
    GPUVertAttr* attr = &format->attrs[attr_id];

    attr->names[attr->name_len++] = copy_attr_name(format, name);
    attr->comp_type = comp_type;
    attr->gl_comp_type = convert_comp_type_to_gl(comp_type);
    attr->comp_len = (comp_type == GPU_COMP_I10) ?
        4 :
        comp_len; /* system needs 10_10_10_2 to be 4 or BGRA */
    attr->sz = attr_sz(attr);
    attr->offset = 0; /* offsets & stride are calculated later (during pack) */
    attr->fetch_mode = fetch_mode;

    return attr_id;
}


uint vertex_buffer_size(const GPUVertFormat* format, uint vertex_len)
{
	//assert(format->packed && format->stride > 0);
	return format->stride * vertex_len;
}