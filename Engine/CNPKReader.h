// Copyright (C) 2002-2012 Nikolaus Gebhardt
// Copyright (C) 2009-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IrrCompileConfig.h"

#ifdef __IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_

#include "IReferenceCounted.h"
#include "IReadFile.h"
#include "irrArray.h"
#include "irrString.h"
#include "IFileSystem.h"
#include "CFileList.h"

namespace irr
{
namespace io
{
namespace
{
//! File header containing location and size of the table of contents
struct SNPKHeader
{
	// Don't change the order of these fields!  They must match the order stored on disk.
	c8 Tag[4];
	u32 Length;
	u32 Offset;
};

//! An entry in the NPK file's table of contents.
struct SNPKFileEntry
{
	core::stringc Name;
	u32 Offset;
	u32 Length;
};
} // end namespace

//! Archiveloader capable of loading Nebula Device 2 NPK Archives
class CArchiveLoaderNPK : public IArchiveLoader
{
public:

	//! Constructor
	CArchiveLoaderNPK(io::IFileSystem* fs);

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".zip")
	virtual bool isALoadableFileFormat(const io::path& filename) const _IRR_OVERRIDE_;

	//! Check if the file might be loaded by this class
	/** Check might look into the file.
	\param file File handle to check.
	\return True if file seems to be loadable. */
	virtual bool isALoadableFileFormat(io::IReadFile* file) const _IRR_OVERRIDE_;

	//! Check to see if the loader can create archives of this type.
	/** Check based on the archive type.
	\param fileType The archive type to check.
	\return True if the archile loader supports this type, false if not */
	virtual bool isALoadableFileFormat(E_FILE_ARCHIVE_TYPE fileType) const _IRR_OVERRIDE_;

	//! Creates an archive from the filename
	/** \param file File handle to check.
	\return Pointer to newly created archive, or 0 upon error. */
	virtual IFileArchive* createArchive(const io::path& filename, bool ignoreCase, bool ignorePaths) const _IRR_OVERRIDE_;

	//! creates/loads an archive from the file.
	//! \return Pointer to the created archive. Returns 0 if loading failed.
	virtual io::IFileArchive* createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const _IRR_OVERRIDE_;

private:
	io::IFileSystem* FileSystem;
};


//! reads from NPK
class CNPKReader : public virtual IFileArchive, virtual CFileList
{
public:

	CNPKReader(IReadFile* file, bool ignoreCase, bool ignorePaths);
	virtual ~CNPKReader();

	// file archive methods

	//! return the id of the file Archive
	virtual const io::path& getArchiveName() const _IRR_OVERRIDE_
	{
		return File->getFileName();
	}

	//! opens a file by file name
	virtual IReadFile* createAndOpenFile(const io::path& filename) _IRR_OVERRIDE_;

	//! opens a file by index
	virtual IReadFile* createAndOpenFile(u32 index) _IRR_OVERRIDE_;

	//! returns the list of files
	virtual const IFileList* getFileList() const _IRR_OVERRIDE_;

	//! get the class Type
	virtual E_FILE_ARCHIVE_TYPE getType() const _IRR_OVERRIDE_
	{
		return EFAT_NPK;
	}

private:

	//! scans for a local header, returns false if the header is invalid
	bool scanLocalHeader();
	void readString(core::stringc& name);

	IReadFile* File;
};

} // end namespace io
} // end namespace irr

#endif // __IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_


