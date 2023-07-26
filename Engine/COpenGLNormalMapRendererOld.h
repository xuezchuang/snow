// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once
#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "IShaderConstantSetCallBack.h"

#include "COpenGLShaderMaterialRendererOld.h"

namespace irr
{
namespace video
{

//! Class for rendering normal maps with OpenGL
class COpenGLNormalMapRendererOld : public COpenGLShaderMaterialRendererOld, public IShaderConstantSetCallBack
{
public:

	//! Constructor
	COpenGLNormalMapRendererOld(video::COpenGLDriverOld* driver,
							 s32& outMaterialTypeNr, E_MATERIAL_TYPE baseMaterial);

	//! Destructor
	~COpenGLNormalMapRendererOld();

	//! Called by the engine when the vertex and/or pixel shader constants for an
	//! material renderer should be set.
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) _IRR_OVERRIDE_;

	//! Returns the render capability of the material.
	virtual s32 getRenderCapability() const _IRR_OVERRIDE_;

protected:

	bool CompiledShaders;
};


} // end namespace video
} // end namespace irr

#endif

