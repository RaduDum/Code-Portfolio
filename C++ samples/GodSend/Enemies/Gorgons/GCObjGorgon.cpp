////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <memory.h>

#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GameSpecific/Player/GCObjProjectilePlayer.h"
#include "GamerCamp/GameSpecific/Player/GCObjGroupProjectilePlayer.h"

#include "GCObjGorgon.h"
#include "Audio/GCSFXManager.h"



USING_NS_CC;

GCFACTORY_IMPLEMENT_CREATEABLECLASS( CGCObjGorgon );

//////////////////////////////////////////////////////////////////////////
// GetGCTypeIDOf uses the template in GCTypeID to generate a unique ID for 
// this type - need this to construct our base type
CGCObjGorgon::CGCObjGorgon()
	: CGCObjEnemy				( GetGCTypeIDOf( CGCObjGorgon ) )
	, m_fLaserTimer				( 0.0f )
	, m_fLaserFireStart			( 1.5f )
	, m_fLaserChargeLimit		( 2.5f )
	, m_fLaserFireLimit			( 4.5f )
	, m_fSwitchLimit			( 5.0f )
	, m_bIsCharging				( false )
	, m_bIsAttacking			( false )
	, m_bIsBreaking				( false )
	, m_fStunModifier			( 1.0f )
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGCObject interface
void CGCObjGorgon::VOnResourceAcquire()
{
	CGCObjEnemy::VOnResourceAcquire();

	CGCSFXManager::GetSharedManager()->LoadSoundEffect( AudioFileNames::pszGorgonResetSound );
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGorgon::VOnResurrected( void )
{
	CGCObjEnemy::VOnResurrected();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGorgon::VOnReset( void )
{
	CGCObjEnemy::VOnReset();
	//reset variables
	m_fLaserTimer			= 0.0f;
	SetFlippedX( false );

	m_bIsBreaking = false;
	m_bIsAttacking = false;
	m_bIsCharging = false;
	
	m_fDirection = 1.0f;

	m_fResetSoundTimer = 0.0f;
}

/////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual
void CGCObjGorgon::VOnUpdate( f32 fTimeStep )
{
	CGCObjEnemy::VOnUpdate( fTimeStep );


	//check if gorgon is stunned
	if( true == GetStunned() )
	{
		//if so remove current behaviours and apply dynamic body for movement
		m_bIsCharging = false;
		m_bIsAttacking = false;
		m_fLaserTimer = 0.0f;

		//check if gorgon is on ground (with slight threshold)
		if( 0.0f <= GetPhysicsBody()->GetLinearVelocity().y
			&& 0.01f > GetPhysicsBody()->GetLinearVelocity().y )
		{
			GetPhysicsBody()->SetLinearVelocity( b2Vec2( GetPhysicsBody()->GetLinearVelocity().x * 0.4f , GetPhysicsBody()->GetLinearVelocity().y ) );
		}

		if( m_fStunTimer > m_fStunLimit - m_fStunModifier && false == m_bIsBreaking  )
		{
			m_bIsBreaking = true;
		}
	}

	//if enemy is not stunned
	else
	{
		if( true == m_bIsBreaking )
		{
			m_bIsBreaking = false;
		}
		//set different state for enemy based on current timer
		if( m_fLaserTimer < m_fSwitchLimit )
		{
			//set charging
			if( m_fLaserTimer > m_fLaserFireStart && m_fLaserTimer < m_fLaserChargeLimit )
			{
				m_bIsCharging = true;
			}

			//set attacking
			else if( m_fLaserTimer > m_fLaserChargeLimit && m_fLaserTimer < m_fLaserFireLimit )
			{
				m_bIsCharging = false;
				m_bIsAttacking = true;
			}

			//stop attacking
			else if( m_fLaserTimer > m_fLaserFireLimit )
			{
				m_bIsAttacking = false;
			}
		}
		else
		{
			GorgonChangeFacing();
		}
		m_fLaserTimer += fTimeStep;
	}
}


//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGorgon::GorgonChangeFacing()
{
	//change gorgon facing direction
	m_fDirection = -1 * m_fDirection;
	SetFlippedX( !IsFlippedX() );
	m_fLaserTimer = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGorgon::ResetTimer( void )
{
	CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszGorgonResetSound );

	m_fLaserTimer = 0.0f;
	SetAttacking( false );
	m_bIsCharging = false;
}