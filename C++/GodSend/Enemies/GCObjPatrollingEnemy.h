#ifndef _GCOBJPATROLLINGENEMY_H_
#define _GCOBJPATROLLINGENEMY_H_

#ifndef _GCOBJSENEMY_H_
#include "GamerCamp/GameSpecific/Enemies/GCObjEnemy.h"
#endif

//patrolling enemy class
//inherits from the CGCObjEnemy
class CGCObjPatrollingEnemy
	: public CGCObjEnemy
{
private:
	//VFX to make the petrified enemy standout
	CGCObjDashableVFX* m_cDashableVFX;
	//check for petrified
	bool m_bIsPetrified;

	//check for attack
	bool m_bIsAttacking;

	//movement variables
	f32 m_fMovementModifier;
	f32 m_fMaxHorizontalSpeed;

public:
	CGCObjPatrollingEnemy( GCTypeID idDerivedType );
	virtual ~CGCObjPatrollingEnemy() {};

	//////////////////////////////////////////////////////////////////////////
	// overridden virtuals from the game object interface
	virtual void	VOnResourceAcquire( void ) override;
	virtual void	VOnResurrected( void ) override;
	virtual void	VOnReset( void ) override;
	virtual void	VOnUpdate( f32 fTimeStep ) override;

	//override function to check for petrified state
	virtual void	VChangeBehaviour( int chCollisionType ) override;

	//enemy movement function used for basic patrolling
	void	EnemyMovement( f32 fTimeStep );

	//function for setting the petrified state
	virtual void SetPetrified( void );

	//getter for petrified state
	inline bool	IsPetrified( void )						{ return m_bIsPetrified; }

	//getter and setter for attacking state
	inline bool	IsAttacking( void )						{ return m_bIsAttacking; }
	inline void	SetAttacking( bool bAttackValue )		{ m_bIsAttacking = bAttackValue; }
};

#endif
