// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "ISceneNodeAnimatorCollisionResponse.h"

namespace irr
{
namespace scene
{

//! Special scene node animator for doing automatic collision detection and response.
/** This scene node animator can be attached to any scene node modifying it in that
way, that it cannot move through walls of the world, is influenced by gravity and
acceleration. This animator is useful for example for first person shooter
games. Attach it for example to a first person shooter camera, and the camera will
behave as the player control in a first person shooter game: The camera stops and
slides at walls, walks up stairs, falls down if there is no floor under it, and so on.
*/
class CSceneNodeAnimatorCollisionResponse : public ISceneNodeAnimatorCollisionResponse
{
public:

	//! constructor
	CSceneNodeAnimatorCollisionResponse(ISceneManager* scenemanager,
										ITriangleSelector* world, ISceneNode* object,
										const core::vector3df& ellipsoidRadius = core::vector3df(30, 60, 30),
										const core::vector3df& gravityPerSecond = core::vector3df(0, -100.0f, 0),
										const core::vector3df& ellipsoidTranslation = core::vector3df(0, 0, 0),
										f32 slidingSpeed = 0.0005f);

	//! destructor
	virtual ~CSceneNodeAnimatorCollisionResponse();

	//! Returns if the attached scene node is falling, which means that
	//! there is no blocking wall from the scene node in the direction of
	//! the gravity.
	virtual bool isFalling() const _IRR_OVERRIDE_;

	//! Sets the radius of the ellipsoid with which collision detection and
	//! response is done.
	virtual void setEllipsoidRadius(const core::vector3df& radius) _IRR_OVERRIDE_;

	//! Returns the radius of the ellipsoid with which the collision detection and
	//! response is done.
	virtual core::vector3df getEllipsoidRadius() const _IRR_OVERRIDE_;

	//! Sets the gravity of the environment.
	virtual void setGravity(const core::vector3df& gravity) _IRR_OVERRIDE_;

	//! 'Jump' the animator, by adding a jump speed opposite to its gravity
	virtual void jump(f32 jumpSpeed) _IRR_OVERRIDE_;

	//! Should the Target react on collision ( default = true )
	virtual void setAnimateTarget(bool enable) _IRR_OVERRIDE_;
	virtual bool getAnimateTarget() const _IRR_OVERRIDE_;

	//! Returns current vector of gravity.
	virtual core::vector3df getGravity() const _IRR_OVERRIDE_;

	//! Sets the translation of the ellipsoid for collision detection.
	virtual void setEllipsoidTranslation(const core::vector3df& translation) _IRR_OVERRIDE_;

	//! Returns the translation of the ellipsoid for collision detection.
	virtual core::vector3df getEllipsoidTranslation() const _IRR_OVERRIDE_;

	//! Sets a triangle selector holding all triangles of the world with which
	//! the scene node may collide.
	virtual void setWorld(ITriangleSelector* newWorld) _IRR_OVERRIDE_;

	//! Returns the current triangle selector containing all triangles for
	//! collision detection.
	virtual ITriangleSelector* getWorld() const _IRR_OVERRIDE_;

	//! animates a scene node
	virtual void animateNode(ISceneNode* node, u32 timeMs) _IRR_OVERRIDE_;

	//! Writes attributes of the scene node animator.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const _IRR_OVERRIDE_;

	//! Reads attributes of the scene node animator.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0) _IRR_OVERRIDE_;

	//! Returns type of the scene node animator
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const _IRR_OVERRIDE_
	{
		return ESNAT_COLLISION_RESPONSE;
	}

	//! Creates a clone of this animator.
	/** Please note that you will have to drop
	(IReferenceCounted::drop()) the returned pointer after calling
	this. */
	virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager = 0) _IRR_OVERRIDE_;

	//! Set the single node that this animator will act on.
	virtual void setTargetNode(ISceneNode* node) _IRR_OVERRIDE_
	{
		setNode(node);
	}

	//! Gets the single node that this animator is acting on.
	virtual ISceneNode* getTargetNode(void) const _IRR_OVERRIDE_
	{
		return Object;
	}

	//! Returns true if a collision occurred during the last animateNode()
	virtual bool collisionOccurred() const _IRR_OVERRIDE_
	{
		return CollisionOccurred;
	}

	//! Returns the last point of collision.
	virtual const core::vector3df& getCollisionPoint() const _IRR_OVERRIDE_
	{
		return CollisionPoint;
	}

	//! Returns the last triangle that caused a collision.
	virtual const core::triangle3df& getCollisionTriangle() const _IRR_OVERRIDE_
	{
		return CollisionTriangle;
	}

	virtual const core::vector3df& getCollisionResultPosition(void) const _IRR_OVERRIDE_
	{
		return CollisionResultPosition;
	}

	virtual ISceneNode* getCollisionNode(void) const _IRR_OVERRIDE_
	{
		return CollisionNode;
	}


	//! Sets a callback interface which will be called if a collision occurs.
	/** \param callback: collision callback handler that will be called when a collision
	occurs. Set this to 0 to disable the callback.
	*/
	virtual void setCollisionCallback(ICollisionCallback* callback) _IRR_OVERRIDE_;

private:

	void setNode(ISceneNode* node);

	core::vector3df Radius;
	core::vector3df Gravity;
	core::vector3df Translation;
	core::vector3df FallingVelocity; // In the direction of Gravity.

	core::vector3df LastPosition;
	core::triangle3df RefTriangle;

	ITriangleSelector* World;
	ISceneNode* Object;
	ISceneManager* SceneManager;
	u32 LastTime;
	f32 SlidingSpeed;

	core::vector3df CollisionPoint;
	core::triangle3df CollisionTriangle;
	core::vector3df CollisionResultPosition;
	ISceneNode* CollisionNode;
	ICollisionCallback* CollisionCallback;

	bool Falling;
	bool IsCamera;
	bool AnimateCameraTarget;
	bool CollisionOccurred;
	bool FirstUpdate;
};

} // end namespace scene
} // end namespace irr


