////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <memory.h>

#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GameSpecific/Player/GCObjProjectilePlayer.h"
#include "GamerCamp/GameSpecific/Player/GCObjGroupProjectilePlayer.h"
#include "cocos2d.h"
#include "GamerCamp/GCCocosInterface/GCCocosHelpers.h"


#include "GCObjMinotaur.h"




USING_NS_CC;

GCFACTORY_IMPLEMENT_CREATEABLECLASS( CGCObjMinotaur );

//////////////////////////////////////////////////////////////////////////
// GetGCTypeIDOf uses the template in GCTypeID to generate a unique ID for 
// this type - need this to construct our base type
CGCObjMinotaur::CGCObjMinotaur()
	: CGCObjPatrollingEnemy(GetGCTypeIDOf(CGCObjMinotaur) )
	, m_fAttackLimit	( 3.0f )
	, m_fAttackTimer	( 0.0f )
	, m_fTiredLimit		( 1.0f )
	, m_fTiredTimer		( 0.0f )
	, m_bTired			( false )
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGCObject interface


//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjMinotaur::VOnResurrected(void)
{
	CGCObjPatrollingEnemy::VOnResurrected();
}

void CGCObjMinotaur::VOnReset(void)
{
	CGCObjPatrollingEnemy::VOnReset();

	//reset timers
	m_fAttackTimer = 0.0f;
	m_fTiredTimer = 0.0f;

	//reset tired state
	m_bTired = false;
}
//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjMinotaur::VOnUpdate( f32 fTimeStep )
{
	CGCObjPatrollingEnemy::VOnUpdate( fTimeStep );

	//check if enemy is not petrified
	if( false == IsPetrified() )
	{

		//check if the minotaur is attacking, whilst not being tired nor stunned
		// if this is the case apply hte movement code
		if( true == IsAttacking() && false == m_bTired && false == GetStunned() )
		{
			float fAccelerationModifier = 0.0f;
			//check for current enemy state if grounded or not
			if( GetNumberOfCollisions() > 0 )
			{
				//if grounded apply higher ground modifier to compensate for friction
				fAccelerationModifier = m_fGroundModifier * m_fDirection;
			}
			else
			{
				//otherwise apply normal modifier
				fAccelerationModifier = m_fAirModifier * m_fDirection;
			}
			//apply the new impulse
			GetPhysicsBody()->ApplyLinearImpulse( b2Vec2( GetPhysicsBody()->GetMass() * ( fAccelerationModifier - GetPhysicsBody()->GetLinearVelocity().x ) , 0 ) , GetPhysicsBody()->GetWorldCenter() );
			
			//decrease timer for attacking
			if( m_fAttackTimer < m_fAttackLimit )
			{
				m_fAttackTimer += fTimeStep;
			}
			//once attacking is finished
			else
			{
				//disable attack states
				SetAttacking( false );
				//enter tired state
				m_bTired = true;
				m_fTiredTimer = 0.0f;
			}
		}
		//check for tired state
		if( true == m_bTired && false == GetStunned() )
		{
			//decrement tired timer
			if( m_fTiredTimer < m_fTiredLimit )
			{
				m_fTiredTimer += fTimeStep;
				//apply deceleration to enemy when tired
				GetPhysicsBody()->ApplyLinearImpulse( b2Vec2( GetPhysicsBody()->GetMass() * ( GetPhysicsBody()->GetLinearVelocity().x / 10.f ) , 0 ) , GetPhysicsBody()->GetWorldCenter() );
			}
			else
			{
				//once enemy has finished being tired reenter normal route
				m_bTired = false;
				if( false == IsAttacking() )
				{
					//swithc direction as well, in order to avoid being trapped in corners
					SetFlippedX( !IsFlippedX() );
					m_fDirection = -m_fDirection;
				}
			}
		}
		//if stunned disable all attack/tired states
		if( true == GetStunned() )
		{
			SetAttacking( false );
			m_bTired = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjMinotaur::VChangeBehaviour( int iCollisionType )
{
	CGCObjPatrollingEnemy::VChangeBehaviour( iCollisionType );
	//check if enemy is petrified before applying behaviours
	if( false == IsPetrified() )
	{
		switch( iCollisionType )
		{
			//attack object
			case EEmyBehav_attack:
			{
				ActivateAttack();
				break;
			}
		}
	}
}


void CGCObjMinotaur::ActivateAttack( void )
{
	//check for petrified and stun before attacking
	if( false == IsPetrified() && false == GetStunned() )
	{
		SetAttacking( true );
		m_fAttackTimer = 0.0f;
	}

}
//check collision with obstacles
void CGCObjMinotaur::NotifyCollisionWithObstacle( b2Vec2 v2ObstaclePosition, f32 fObstacleWidth )
{

	GetVFXSprite()->SetFlippedX( IsFlippedX() );

	f32 fVFXHorizontalPosition = v2ObstaclePosition.x - fObstacleWidth * 0.55f * m_fDirection;

	GetVFXSprite()->SetPosition( b2Vec2( fVFXHorizontalPosition,
		GetPosition().y ) );

	GetVFXSprite()->StopAllActions();

	GetVFXSprite()->RunAction(
		GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "VFX_Obstacle" ) ) );

}