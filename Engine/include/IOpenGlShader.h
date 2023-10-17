// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "irrString.h"
#include "matrix4.h"

namespace irr
{
namespace video
{
class IOpenGlShader
{
public:
	IOpenGlShader(){}

	virtual ~IOpenGlShader()
	{

	}
	virtual void use() = 0;
	virtual void setBool(const core::stringc& name, bool value) const = 0;
	virtual void setInt(const core::stringc& name, s32 value) const = 0;
	virtual void setFloat(const core::stringc& name, f32 value) const = 0;
	virtual void setMat4(const core::stringc& name, const core::matrix4& mat) const = 0;
};

} // end namespace scene
} // end namespace irr


