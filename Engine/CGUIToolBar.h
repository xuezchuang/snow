// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIToolbar.h"

namespace irr
{
namespace gui
{

//! Stays at the top of its parent like the menu bar and contains tool buttons
class CGUIToolBar : public IGUIToolBar
{
public:

	//! constructor
	CGUIToolBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	//! Updates the absolute position.
	virtual void updateAbsolutePosition() _IRR_OVERRIDE_;

	//! Adds a button to the tool bar
	virtual IGUIButton* addButton(s32 id = -1, const wchar_t* text = 0, const wchar_t* tooltiptext = 0,
								  video::ITexture* img = 0, video::ITexture* pressed = 0,
								  bool isPushButton = false, bool useAlphaChannel = false) _IRR_OVERRIDE_;

private:

	s32 ButtonX;
};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_


