////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GCOBJGORGON_H_
#define _GCOBJGORGON_H_

#include "../../../GCCocosInterface/GCFactory_ObjSpritePhysics.h"

#ifndef _GCOBJENEMY_H_
#include "GamerCamp/GameSpecific/Enemies/GCObjEnemy.h"
#endif

#ifndef	_GCOBJENEMYBEHAVIOURS_H_
#include "GamerCamp/GameSpecific/Minotaurs/GCObjEnemyBehaviours.h"
#endif

//////////////////////////////////////////////////////////////////////////
// This is a sample class derived from CGCObject.
// 
// It could be the basis of your Gorgonobject, it's up to you really.
//
//////////////////////////////////////////////////////////////////////////
class CGCObjGorgon
	: public CGCObjEnemy
{
private:
	//timelimit used for determining when gorgon changes directions
	f32 m_fSwitchLimit;

	//limits used for showing gorgon's telegraph
	f32 m_fLaserTimer;
	f32 m_fLaserFireStart;
	// limits used for checking the gorgon's laser attack
	f32 m_fLaserChargeLimit;
	f32 m_fLaserFireLimit;

	//modifier for medusa breaking free from stun
	f32 m_fStunModifier;
	
	//check if gorgon is attacking or charging
	bool m_bIsCharging;

	//check if gorgoni s attacking
	bool m_bIsAttacking;

	//gorgon is breaking free
	bool m_bIsBreaking;

	//reset sound timer
	f32 m_fResetSoundTimer;
	const f32 k_fResetSoundTimerLimit = 0.5f;
public:
	GCFACTORY_DECLARE_CREATABLECLASS( CGCObjGorgon );

	CGCObjGorgon();
	//////////////////////////////////////////////////////////////////////////
	// we need a virtual destructor since delete will be called on pointers of 
	// this class to delete derived types.
	virtual ~CGCObjGorgon() {};

	//////////////////////////////////////////////////////////////////////////
	// overridden virtuals from the enemy and sprite physics interface

	virtual void	VOnResourceAcquire		( void ) override;
	virtual void	VOnResurrected			( void ) override;
	virtual void	VOnReset				( void ) override;
	virtual void	VOnUpdate				( f32 fTimeStep ) override;

	void			GorgonChangeFacing		( void );

	//reset the switch timer for enemy on collision with player's dash
	void			ResetTimer				( void );

	//get function for charging
	inline bool			GetCharging				( void ) { return m_bIsCharging; }
	
	//get function for charging
	inline bool			GetBreaking				( void ) { return m_bIsBreaking; }

	//get function for charging
	inline bool			GetAttacking			( void ) { return m_bIsAttacking; }

	
	//set function for attacking
	inline void			SetAttacking			( bool bAttackValue )		{ m_bIsAttacking = bAttackValue; }
};
#endif // #ifndef _GCOBJGORGON_H_