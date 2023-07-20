// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "irrArray.h"
#include "ISceneNodeAnimatorFinishing.h"

namespace irr
{
namespace scene
{
class CSceneNodeAnimatorTexture : public ISceneNodeAnimatorFinishing
{
public:

	//! constructor
	CSceneNodeAnimatorTexture(const core::array<video::ITexture*>& textures,
							  s32 timePerFrame, bool loop, u32 now);

	//! destructor
	virtual ~CSceneNodeAnimatorTexture();

	//! animates a scene node
	virtual void animateNode(ISceneNode* node, u32 timeMs) _IRR_OVERRIDE_;

	//! Writes attributes of the scene node animator.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const _IRR_OVERRIDE_;

	//! Reads attributes of the scene node animator.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0) _IRR_OVERRIDE_;

	//! Returns type of the scene node animator
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const _IRR_OVERRIDE_
	{
		return ESNAT_TEXTURE;
	}

	//! Creates a clone of this animator.
	/** Please note that you will have to drop
	(IReferenceCounted::drop()) the returned pointer after calling
	this. */
	virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager = 0) _IRR_OVERRIDE_;


private:

	void clearTextures();

	core::array<video::ITexture*> Textures;
	u32 TimePerFrame;
	bool Loop;
};


} // end namespace scene
} // end namespace irr


