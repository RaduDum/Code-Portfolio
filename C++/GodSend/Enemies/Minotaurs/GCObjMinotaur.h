////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GCOBJMINOTAUR_H_
#define _GCOBJMINOTAUR_H_

#include "../../../GCCocosInterface/GCFactory_ObjSpritePhysics.h"

#ifndef _GCOBJPATROLLINGENEMY_H_
#include "GamerCamp/GameSpecific/Enemies/GCObjPatrollingEnemy.h"
#endif


#ifndef	_GCOBJENEMYBEHAVIOURS_H_
#include "GamerCamp/GameSpecific/Minotaurs/GCObjEnemyBehaviours.h"
#endif

//////////////////////////////////////////////////////////////////////////
// This is a sample class derived from CGCObject.
// 
// It could be the basis of your Minotaur object, it's up to you really.
//
//////////////////////////////////////////////////////////////////////////
class CGCObjMinotaur
	: public CGCObjPatrollingEnemy
{
private:
	//timer for attacking
	f32 m_fAttackTimer;
	f32 m_fAttackLimit;

	//tired timer
	f32 m_fTiredTimer;
	f32 m_fTiredLimit;

	//modifier used to keep track of different movement rates based on current enemy location
	// this is used to compensate for the friction and lack of priction mid air
	const f32 m_fGroundModifier = 15.5f;
	const f32 m_fAirModifier = 7.0f;

	//boolean used for keeping track of tired state
	bool m_bTired;

public:
	GCFACTORY_DECLARE_CREATABLECLASS( CGCObjMinotaur );

	CGCObjMinotaur();
	//////////////////////////////////////////////////////////////////////////
	// we need a virtual destructor since delete will be called on pointers of 
	// this class to delete derived types.
	virtual ~CGCObjMinotaur() {};

	//////////////////////////////////////////////////////////////////////////
	// overridden virtuals from the game object interface
	virtual void	VOnResurrected( void ) override;
	virtual void	VOnReset( void ) override;
	virtual void	VOnUpdate( f32 fTimeStep ) override;

	virtual void	VChangeBehaviour( int iCollisionType ) override;

	//attack activation
	void	ActivateAttack( void );
	
	void	NotifyCollisionWithObstacle( b2Vec2 v2ObstaclePosition, f32 fObstacleWidth );
};

#endif // #ifndef _GCOBJMINOTAUR_H_