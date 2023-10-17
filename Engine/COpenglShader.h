#pragma once

#include "IOpenGlShader.h"
//#include "irrString.h"
//#include "IReadFile.h"

namespace irr
{
namespace video
{
class COpenglShader : public IOpenGlShader
{
public:
    u32 ID;
	//unsigned int ID;
    COpenglShader(io::IReadFile* vs, io::IReadFile* fs);
    virtual ~COpenglShader();
	virtual void use() _IRR_OVERRIDE_;
	virtual void setBool(const core::stringc& name, bool value) const _IRR_OVERRIDE_;
	virtual void setInt(const core::stringc& name, s32 value) const _IRR_OVERRIDE_;
	virtual void setFloat(const core::stringc& name, f32 value) const _IRR_OVERRIDE_;
	virtual void setMat4(const core::stringc& name, const core::matrix4& mat) const _IRR_OVERRIDE_;
private:
	void checkCompileErrors(u32 shader, const core::stringc& type);
	//void checkCompileErrors(unsigned int shader, const core::stringc& type);
	void readstringc(io::IReadFile* file, core::stringc& shader);
};

}
}