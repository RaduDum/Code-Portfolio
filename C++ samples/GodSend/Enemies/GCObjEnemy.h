////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GCOBJENEMY_H_
#define _GCOBJENEMY_H_

#ifndef _GCOBJSPRITEPHYSICS_H_
	#include "../../GCCocosInterface/GCObjSpritePhysics.h"
#endif

#ifndef	_GCOBJENEMYBEHAVIOURS_H_
	#include "GamerCamp/GameSpecific/Enemies/GCObjEnemyBehaviours.h"
#endif

#include "GamerCamp/GCCocosInterface/SGCFactoryCreationParams.h"
#include "../DashableVFX/GCObjDashableVFX.h"

//enum used to track various different animation states enemies can have
enum EEnemyAnimationStates{
	EEmyAniS_walk,
	EEmyAniS_idle,
	EEmyAniS_petrified,
	EEmyAniS_charge,
	EEmyAniS_attack,
	EEmyAniS_stun,
	EEmyAniS_death,
	EEmyAniS_breakfree,
	EEmyAniS_none
};

class CGCObjItem;


//////////////////////////////////////////////////////////////////////////
// This is a sample class derived from CGCObject.
// 
// It could be the basis of your Cyclopsobject, it's up to you really.
//
//////////////////////////////////////////////////////////////////////////
class CGCObjEnemy
	: public CGCObjSpritePhysics
{
private:
	//VFX when the enemy dies
	CGCObjDashableVFX* m_cDeathVFX;


	//VFX when the enemy dies
	CGCObjDashableVFX* m_cHitVFX;

	CGCObjSprite* m_pcVFXSprite;

	//booleans used for cehcking different staes of the enemy
	//check for dead and stunned

	bool m_bIsDead;
	bool m_bIsStunned;
	//booleans used for cehcking different staes of the enemy
	//check for attacking
	bool m_bIsAttacking;

	//variable used to track starting enemy facing
	bool m_bDefaultXFacing;

	//knockback timer and limit
	f32 m_fKnockedBackTimer;
	f32 m_fKnockedBackTimerLimit;

	//variable used to track current animation based on the EEnemyAnimationStates enum
	i32 m_iCurrentAnimation;

	//push timer and limit
	f32 m_fVerticalPushSpeed;
	f32 m_fHorizontalPushSpeed;

	//direction switch timer and limit
	f32 m_fEnemySwitchTimer;
	f32 m_fEnemySwitchLimit;

	//pushing enemy timer
	f32 m_fPushTimer;
	f32 m_fPushTimerLimit;

	//death timer, keeps track of when enemy dies and when he should be removed from play
	f32	m_fDeathTimer;
	f32	m_fDeathLimit;

	//boolean used to track if an enemy has a collectable
	bool m_bHasCollectable;

	//number of ground collision enemy has
	int m_iNumberOfCollisions;

	//scroll along with ogmo parameters 
	CGCFactoryCreationParams m_sScrollParams;
	CGCObjItem* m_pcScroll;

	
public:
	CGCObjEnemy(GCTypeID idDerivedType);

	//collectable tracking enemy location based on collectable
	CGCObjSprite* m_cCollectableBubble;

	//float used to keep track of direction
	//has values of 1(left) and -1(right)
	f32 m_fDirection;

	//timer used to keep track of stun
	f32 m_fStunTimer;
	f32 m_fStunLimit;
	
	//////////////////////////////////////////////////////////////////////////
	// we need a virtual destructor since delete will be called on pointers of 
	// this class to delete derived types.
	virtual ~CGCObjEnemy() {};

	//////////////////////////////////////////////////////////////////////////
	// overridden virtuals from the game object interface
	virtual void	VOnResourceAcquire( void ) override;
	virtual void	VOnResourceRelease( void ) override;
	virtual void	VOnResurrected( void ) override;
	virtual void	VOnReset( void ) override;
	virtual void	VOnUpdate( f32 fTimeStep ) override;
	
	//new virtual method used to keep track of enemy behaviours
	virtual void	VChangeBehaviour( int chCollisionType );

	//set default physics parameters for all enemies
	void	SetPhysicsParameters( void );

	inline CGCObjSprite* GetVFXSprite() { return m_pcVFXSprite; }

	//apply stun
	void	ActivateStun( void );

	//apply push
	void	Push( float fPushValue );

	//apply knockback
	void    ApplyKnockBack( f32 fKnockBackDirection );
		
	//getters and setters for death state
	inline bool		GetDead( void )					{ return m_bIsDead; }
	inline void		SetDead( bool bDead )			{ m_bIsDead = bDead; }

	//getters and setters for stun
	inline bool		GetStunned( void )				{ return m_bIsStunned; }
	inline void		SetStunned( bool bStunValue )	{ m_bIsStunned = bStunValue; }

	//get current animation state from the enemy
	inline int		GetAnimation( void )			{ return m_iCurrentAnimation; }
	inline void		SetAnimation( i32 iAnim )		{ m_iCurrentAnimation = iAnim; }
	
	//get number of collisions
	inline	int		GetNumberOfCollisions( void )	{ return m_iNumberOfCollisions; }

};
#endif // #ifndef _GCOBJENEMY_H_