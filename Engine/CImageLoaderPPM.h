// Copyright (C) 2007-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PPM_LOADER_

#include "IImageLoader.h"
#include "irrString.h"


namespace irr
{
namespace video
{


/*!
	Surface Loader for SUN Pixmaps
*/
class CImageLoaderPPM : public IImageLoader
{
public:

	//! constructor
	CImageLoaderPPM();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const io::path& filename) const _IRR_OVERRIDE_;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(io::IReadFile* file) const _IRR_OVERRIDE_;

	//! creates a surface from the file
	virtual IImage* loadImage(io::IReadFile* file) const _IRR_OVERRIDE_;

private:
	//! read the next token from file
	void getNextToken(io::IReadFile* file, core::stringc& token) const;
	//! skip to next token (skip whitespace)
	void skipToNextToken(io::IReadFile* file) const;
};

} // end namespace video
} // end namespace irr


#endif

