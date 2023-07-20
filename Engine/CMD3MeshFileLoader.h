// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "IQ3Shader.h"

namespace irr
{
namespace scene
{

//! Meshloader capable of loading md3 files.
class CMD3MeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	CMD3MeshFileLoader(scene::ISceneManager* smgr);

	//! destructor
	virtual ~CMD3MeshFileLoader();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".bsp")
	virtual bool isALoadableFileExtension(const io::path& filename) const _IRR_OVERRIDE_;

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) _IRR_OVERRIDE_;

private:
	scene::ISceneManager* SceneManager;

};

} // end namespace scene
} // end namespace irr


