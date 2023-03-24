#include <memory.h>

#include "Audio/GCSFXManager.h"
#include "GamerCamp/GCObject/GCObjectManager.h"

#include "GCObjPatrollingEnemy.h"


CGCObjPatrollingEnemy::CGCObjPatrollingEnemy( GCTypeID idDerivedType )
	: CGCObjEnemy( idDerivedType )
	, m_fMovementModifier( 0.5f )
	, m_fMaxHorizontalSpeed( 1.9f )
	, m_bIsAttacking( false )
	, m_bIsPetrified( false )
	, m_cDashableVFX( new CGCObjDashableVFX( "Pushable_VFX_Idle", "TexturePacker/Sprites/Common/Pushable_VFX/Pushable_VFX.plist" ) )
{
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjPatrollingEnemy::VOnResourceAcquire( void )
{
	CGCObjEnemy::VOnResourceAcquire();

	CGCSFXManager* pSFXManager = CGCSFXManager::GetSharedManager();
	pSFXManager->LoadSoundEffect( AudioFileNames::pszEnemyPetrified );
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 

void CGCObjPatrollingEnemy::VOnResurrected( void )
{
	CGCObjEnemy::VOnResurrected();
}
//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 

void CGCObjPatrollingEnemy::VOnReset( void )
{
	CGCObjEnemy::VOnReset();
	//reset variables for patrolling enemies
	m_fMovementModifier = 0.5f;
	m_bIsAttacking = false;
	m_bIsPetrified = false;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjPatrollingEnemy::VOnUpdate( f32 fTimeStep )
{
	CGCObjEnemy::VOnUpdate( fTimeStep );
	//check to see if enemy is petrified
	if( false == m_bIsPetrified  )
	{
		//allow movement if enemy is on floor and not stunned
		if( false == GetStunned() && false == m_bIsAttacking && false == GetDead() )
		{
			EnemyMovement( fTimeStep );
		}
	}
	//the vfx should follow the enemy
	m_cDashableVFX->VOnUpdate( fTimeStep, b2Vec2( this->GetPosition().x, this->GetPosition().y ) );
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjPatrollingEnemy::EnemyMovement( f32 fTimeStep )
{
	b2Vec2 rv2NewVelocity = this->GetPhysicsBody()->GetLinearVelocity();

	float m_fHorizontalVelocity = 0.1f;

	//apply direction based on enemy facing
	if( GetNumberOfCollisions() > 0 )
	{
		m_fHorizontalVelocity = 1.35f;
		//create  max velocity taking into account the current player direction and current spacial position
		f32 fMaxVelocity = m_fDirection * ( m_fMaxHorizontalSpeed + m_fHorizontalVelocity );

		//apply new velocity
		m_fHorizontalVelocity = fMaxVelocity;

		GetPhysicsBody()->ApplyLinearImpulse( b2Vec2( GetPhysicsBody()->GetMass() * ( m_fHorizontalVelocity - rv2NewVelocity.x ) , 0 ) , GetPhysicsBody()->GetWorldCenter() );
	}
	else
	{
		//different velocity is calculate for deceleration for edge cases where no input is detected
		m_fHorizontalVelocity = ( rv2NewVelocity.x * 0.1f - rv2NewVelocity.x );
		GetPhysicsBody()->ApplyLinearImpulse( b2Vec2( GetPhysicsBody()->GetMass() * m_fHorizontalVelocity , 0 ) ,
			GetPhysicsBody()->GetWorldCenter() );
	}

}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
//set stunned
void CGCObjPatrollingEnemy::SetPetrified( void )
{
	//check that hte physics body exists before applyig petrified change
	if( this->GetPhysicsBody() != nullptr )
	{
		//boolean set to true, used to disable other functionality
		m_bIsPetrified = true;
		CGCSFXManager* pSFXManager = CGCSFXManager::GetSharedManager();
		pSFXManager->PlaySoundEffect( AudioFileNames::pszEnemyPetrified );
		m_cDashableVFX->PlayVFX();
		//go through each enemy figure and change the friction and density so it resembles a pushable block
		for( b2Fixture* pFixture = this->GetPhysicsBody()->GetFixtureList();
			pFixture != nullptr;
			pFixture = pFixture->GetNext() )
		{
			pFixture->SetDensity( 30.0f );
			pFixture->SetFriction( 20.0f );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjPatrollingEnemy::VChangeBehaviour( int chCollisionType )
{
	//check for petrified state before changing behaviours
	if( false == m_bIsPetrified )
	{
		CGCObjEnemy::VChangeBehaviour( chCollisionType );
	}
}