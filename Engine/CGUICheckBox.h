// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#pragma once

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUICheckBox.h"

namespace irr
{
namespace gui
{

class CGUICheckBox : public IGUICheckBox
{
public:

	//! constructor
	CGUICheckBox(bool checked, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

	//! set if box is checked
	virtual void setChecked(bool checked) _IRR_OVERRIDE_;

	//! returns if box is checked
	virtual bool isChecked() const _IRR_OVERRIDE_;

	//! Sets whether to draw the background
	virtual void setDrawBackground(bool draw) _IRR_OVERRIDE_;

	//! Checks if background drawing is enabled
	/** \return true if background drawing is enabled, false otherwise */
	virtual bool isDrawBackgroundEnabled() const _IRR_OVERRIDE_;

	//! Sets whether to draw the border
	virtual void setDrawBorder(bool draw) _IRR_OVERRIDE_;

	//! Checks if border drawing is enabled
	/** \return true if border drawing is enabled, false otherwise */
	virtual bool isDrawBorderEnabled() const _IRR_OVERRIDE_;

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	//! Writes attributes of the element.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const _IRR_OVERRIDE_;

	//! Reads attributes of the element
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) _IRR_OVERRIDE_;

private:

	u32 CheckTime;
	bool Pressed;
	bool Checked;
	bool Border;
	bool Background;
};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_CHECKBOX_H_INCLUDED__

