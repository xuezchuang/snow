// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#pragma once

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIFileOpenDialog.h"
#include "IGUIButton.h"
#include "IGUIListBox.h"
#include "IGUIEditBox.h"
#include "IFileSystem.h"

namespace irr
{
namespace gui
{

class CGUIFileOpenDialog : public IGUIFileOpenDialog
{
public:

	//! constructor
	CGUIFileOpenDialog(const wchar_t* title, IGUIEnvironment* environment,
					   IGUIElement* parent, s32 id, bool restoreCWD = false,
					   io::path::char_type* startDir = 0);

	//! destructor
	virtual ~CGUIFileOpenDialog();

	//! returns the filename of the selected file. Returns NULL, if no file was selected.
	virtual const wchar_t* getFileName() const _IRR_OVERRIDE_;

	//! Returns the filename of the selected file. Is empty if no file was selected.
	virtual const io::path& getFileNameP() const _IRR_OVERRIDE_;

	//! Returns the directory of the selected file. Returns NULL, if no directory was selected.
	virtual const io::path& getDirectoryName() const _IRR_OVERRIDE_;

	//! Returns the directory of the selected file converted to wide characters. Returns NULL if no directory was selected.
	virtual const wchar_t* getDirectoryNameW() const _IRR_OVERRIDE_;

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const _IRR_OVERRIDE_;
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0) _IRR_OVERRIDE_;

protected:

	void setFileName(const irr::io::path& name);
	void setDirectoryName(const irr::io::path& name);

	//! Ensure filenames are converted correct depending on wide-char settings
	void pathToStringW(irr::core::stringw& result, const irr::io::path& p);

	//! fills the listbox with files.
	void fillListBox();

	//! sends the event that the file has been selected.
	void sendSelectedEvent(EGUI_EVENT_TYPE type);

	//! sends the event that the file choose process has been canceld
	void sendCancelEvent();

	core::position2d<s32> DragStart;
	io::path FileName;
	core::stringw FileNameW;
	io::path FileDirectory;
	io::path FileDirectoryFlat;
	core::stringw FileDirectoryFlatW;
	io::path RestoreDirectory;
	io::path StartDirectory;

	IGUIButton* CloseButton;
	IGUIButton* OKButton;
	IGUIButton* CancelButton;
	IGUIListBox* FileBox;
	IGUIEditBox* FileNameText;
	IGUIElement* EventParent;
	io::IFileSystem* FileSystem;
	io::IFileList* FileList;
	bool Dragging;
};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

