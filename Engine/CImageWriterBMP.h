// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once
#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_BMP_WRITER_

#include "IImageWriter.h"

namespace irr
{
namespace video
{

class CImageWriterBMP : public IImageWriter
{
public:
	//! constructor
	CImageWriterBMP();

	//! return true if this writer can write a file with the given extension
	virtual bool isAWriteableFileExtension(const io::path& filename) const _IRR_OVERRIDE_;

	//! write image to file
	virtual bool writeImage(io::IWriteFile* file, IImage* image, u32 param) const _IRR_OVERRIDE_;
};

} // namespace video
} // namespace irr

#endif

