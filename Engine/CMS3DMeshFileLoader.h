// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IMeshLoader.h"
#include "IVideoDriver.h"
#include "CSkinnedMesh.h"

namespace irr
{
namespace scene
{

//! Meshloader capable of loading Milkshape 3D files
class CMS3DMeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	CMS3DMeshFileLoader(video::IVideoDriver* driver);

	//! returns true if the file might be loadable by this class
	//! based on the file extension (e.g. ".bsp")
	virtual bool isALoadableFileExtension(const io::path& filename) const _IRR_OVERRIDE_;

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) _IRR_OVERRIDE_;

private:

	core::stringc stripPathFromString(const core::stringc& inString, bool returnPath) const;

	bool load(io::IReadFile* file);
	video::IVideoDriver* Driver;
	CSkinnedMesh* AnimatedMesh;
};

} // end namespace scene
} // end namespace irr



