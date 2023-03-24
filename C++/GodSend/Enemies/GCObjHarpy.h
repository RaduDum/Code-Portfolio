#ifndef _GCOBJHARPY_H_
#define _GCOBJHARPY_H_

#include "../../GCCocosInterface/GCObjSpritePhysics.h"
#include "../../GCCocosInterface/GCFactory_ObjSpritePhysics.h"

class CGCObjHarpy
	: public CGCObjSpritePhysics
{
private:
	b2Vec2	m_v2VelocityDesired;			// velocity of the platform
	b2Vec2	m_v2StartingPosition;		//temporary end position of the platform
	b2Vec2	m_v2EndPosition;				// final position of the platform

	//cooldown timer to allow the platform to move away from the object it is bumping to before switching direction again
	f32 m_fCollisionCooldownTimer;
	f32 m_fCollisionCooldownTimerLimit;
	
	//cooldown timer to allow the platform to move away from the object it is bumping to before switching direction again
	f32 m_fDeathTimer;
	f32 m_fDeathLimit;

	bool m_bDied;
	bool m_bAnimationSwitched;
	//reverses the direction of the platform
	void ReverseDirection();

	void OnDead();

	//linear velocity of the platform
	b2Vec2 m_v2LinearVelocity;
	bool m_bFacingDirection;
public:
	CGCObjHarpy( /*b2Vec2 v2StartingPosition, b2Vec2 v2EndPosition*/ );

	virtual ~CGCObjHarpy() {};

	GCFACTORY_DECLARE_CREATABLECLASS( CGCObjHarpy );

	//virtual void VOnResourceAcquire( void ) override;

	virtual void VOnUpdate( f32 fTimeStep );
	virtual void VOnReset();
	virtual void VOnResourceAcquire();

	bool UpdateMovement( f32 fTimesTep );

	inline void SetDead( bool bHasDied )
	{
		m_bDied = bHasDied;
	}
	inline bool GetDead(  )
	{
		return m_bDied;
	}
	inline void SetEndPosition( b2Vec2 v2EndPosition )
	{
		m_v2EndPosition = v2EndPosition;
	}

	inline b2Vec2 GetHarpyVelocity()
	{
		return  m_v2LinearVelocity;
	}

	virtual void VHandleFactoryParams( const CGCFactoryCreationParams& rCreationParams, b2Vec2 v2InitialPosition, f32 fInitialPosition ) override;
};

#endif