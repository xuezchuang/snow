// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "SoftwareDriver2_compile_config.h"

#include "IMaterialRenderer.h"
#include "CSoftwareDriver2.h"

namespace irr
{
namespace video
{

//! Base class for all internal Software2 material renderers
class CSoftware2MaterialRenderer : public IMaterialRenderer
{
public:

	//! Constructor
	CSoftware2MaterialRenderer(video::CBurningVideoDriver* driver)
		: Driver(driver)
	{
	}

protected:

	video::CBurningVideoDriver* Driver;
};

//! solid material renderer
class CSoftware2MaterialRenderer_SOLID : public CSoftware2MaterialRenderer
{
public:
	CSoftware2MaterialRenderer_SOLID(video::CBurningVideoDriver* driver)
		:CSoftware2MaterialRenderer(driver)
	{
	}

	//! Returns if the material is transparent.
	virtual bool isTransparent() const _IRR_OVERRIDE_
	{
		return false;
	}

};



//! Transparent material renderer
class CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR : public CSoftware2MaterialRenderer
{
public:
	CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR(video::CBurningVideoDriver* driver)
		: CSoftware2MaterialRenderer(driver)
	{
	}


	//! Returns if the material is transparent.
	virtual bool isTransparent() const _IRR_OVERRIDE_
	{
		return true;
	}

};

//! unsupported material renderer
class CSoftware2MaterialRenderer_UNSUPPORTED : public CSoftware2MaterialRenderer
{
public:
	CSoftware2MaterialRenderer_UNSUPPORTED(video::CBurningVideoDriver* driver)
		: CSoftware2MaterialRenderer(driver)
	{
	}

	virtual s32 getRenderCapability() const _IRR_OVERRIDE_
	{
		return 1;
	}

};

//! unsupported material renderer
class CBurningShader_REFERENCE : public CSoftware2MaterialRenderer
{
public:
	CBurningShader_REFERENCE(video::CBurningVideoDriver* driver)
		: CSoftware2MaterialRenderer(driver)
	{
	}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
							   bool resetAllRenderstates, IMaterialRendererServices* services) _IRR_OVERRIDE_
	{
	}

	virtual void OnUnsetMaterial() _IRR_OVERRIDE_
	{
	}

	virtual bool isTransparent() const _IRR_OVERRIDE_
	{
		return false;
	}

	virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype) _IRR_OVERRIDE_
	{
		return true;
	};


	virtual s32 getRenderCapability() const _IRR_OVERRIDE_
	{
		return 1;
	}

};



} // end namespace video
} // end namespace irr


