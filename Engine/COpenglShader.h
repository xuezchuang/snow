#pragma once

#include "IOpenGlShader.h"

namespace irr
{
namespace video
{
class COpenglShader : IOpenGlShader
{
public:
    unsigned int ID;
    COpenglShader(const core::stringc& vertexPath, const core::stringc& fragmentPath);
    virtual ~COpenglShader();
	virtual void use()override;
	virtual void setBool(const core::stringc& name, bool value) const override;
	virtual void setInt(const core::stringc& name, s32 value) const override;
	virtual void setFloat(const core::stringc& name, f32 value) const override;

private:
	void checkCompileErrors(u32 shader, const core::stringc& type);
};

}
}