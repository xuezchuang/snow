// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIElementFactory.h"

namespace irr
{
namespace gui
{
class IGUIElement;
class IGUIEnvironment;

//! This interface makes it possible to dynamically create gui elements.
class CDefaultGUIElementFactory : public IGUIElementFactory
{
public:

	CDefaultGUIElementFactory(IGUIEnvironment* env);

	//! Adds an element to the gui environment based on its type id.
	/** \param type: Type of the element to add.
	\param parent: Parent scene node of the new element. A value of 0 adds it to the root.
	\return Returns pointer to the new element or 0 if unsuccessful. */
	virtual IGUIElement* addGUIElement(EGUI_ELEMENT_TYPE type, IGUIElement* parent = 0) _IRR_OVERRIDE_;

	//! Adds a GUI element to the GUI Environment based on its type name.
	/** \param typeName: Type name of the element to add. Taken from the GUIElementTypeNames c8* array.
	\param parent: Parent scene node of the new element. A value of 0 adds it to the root.
	\return Returns pointer to the new element or 0 if unsuccessful. */
	virtual IGUIElement* addGUIElement(const c8* typeName, IGUIElement* parent = 0) _IRR_OVERRIDE_;

	//! Returns the amount of GUI element types this factory is able to create.
	virtual s32 getCreatableGUIElementTypeCount() const _IRR_OVERRIDE_;

	//! Returns the type of a createable GUI element type based on the index.
	/** \param idx: Index of the element type in this factory. The value must be equal or greater than 0
	and lower than getCreatableGUIElementTypeCount(). */
	virtual EGUI_ELEMENT_TYPE getCreateableGUIElementType(s32 idx) const _IRR_OVERRIDE_;

	//! Returns the type name of a createable GUI element type based on the index.
	/** \param idx: Index of the element type in this factory. The value must be equal or greater than 0
	and lower than getCreatableGUIElementTypeCount(). */
	virtual const c8* getCreateableGUIElementTypeName(s32 idx) const _IRR_OVERRIDE_;

	//! Returns the type name of a createable GUI element based on its type.
	/** \param type: Type of the GUI element.
	\return: Returns the name of the type if this factory can create it, otherwise it returns 0. */
	virtual const c8* getCreateableGUIElementTypeName(EGUI_ELEMENT_TYPE type) const _IRR_OVERRIDE_;

private:

	EGUI_ELEMENT_TYPE getTypeFromName(const c8* name) const;

	IGUIEnvironment* Environment;
};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

