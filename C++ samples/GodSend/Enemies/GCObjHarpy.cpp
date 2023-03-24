#include "GCObjHarpy.h"

#include "GamerCamp/GCCocosInterface/GCCocosHelpers.h"
#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GCCocosInterface/IGCGameLayer.h"
#include "../../GCCocosInterface/GB2ShapeCache-x.h"

#include "GamerCamp/GCCocosInterface/FactoryParameters/GCMovingPlatformFactoryParams.h"

USING_NS_CC;

CGCObjHarpy::CGCObjHarpy( /*b2Vec2 v2StartingPosition, b2Vec2 v2EndPosition*/ )
	: CGCObjSpritePhysics( GetGCTypeIDOf( CGCObjHarpy ) )
	, m_v2VelocityDesired( 0.f, 10.f )
	, m_v2EndPosition( b2Vec2( 0.f, 0.f ) )
	, m_v2StartingPosition( b2Vec2( 0.f, 0.f ) )
	, m_fCollisionCooldownTimer( 1.0f )
	, m_fCollisionCooldownTimerLimit( 1.0f )
	, m_v2LinearVelocity( 0.0f, 0.0f )
	, m_bFacingDirection( false )
	, m_fDeathTimer( 0.0f )
	, m_fDeathLimit( 1.0f )
	, m_bDied( false )
	, m_bAnimationSwitched( false )
{
}

GCFACTORY_IMPLEMENT_CREATEABLECLASS( CGCObjHarpy );

//virtual 
void CGCObjHarpy::VOnResourceAcquire( void )
{
	CGCObjSpritePhysics::VOnResourceAcquire();

	GetSprite()->setZOrder( 6 );

	const char* pszPlist = GetFactoryCreationParams()->strPlistFile.c_str();

	// animate!
	// N.B. this dictionary is NOT autoreleased and we must remember to release it or it will leak
	CCDictionary* pdictPList = GCCocosHelpers::CreateDictionaryFromPlist( pszPlist );
	CCAnimation* pAnimationIdleHarpy = GCCocosHelpers::CreateAnimation( pdictPList, "IdleHarpy" );
	CCAnimation* pAnimationDeadHarpy = GCCocosHelpers::CreateAnimation( pdictPList , "DeadHarpy" );

	//loads in sprite frames
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile( pszPlist );

	//add animations to the shared cache for future use(this way only create the animation once)
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationIdleHarpy , "IdleHarpy" );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationDeadHarpy , "DeadHarpy" );

	this->StopAllActions();
	this->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "IdleHarpy" ) ) );

}

void CGCObjHarpy::VOnUpdate( f32 fTimeStep )
{
	if( false == m_bDied )
	{//update the cooldown timer if not already full (to avoid overflows)
		if( b2_dynamicBody == GetPhysicsBody()->GetType() )
		{
			GetPhysicsBody()->SetType( b2_kinematicBody );
		}

		if( m_fCollisionCooldownTimer < m_fCollisionCooldownTimerLimit )
		{
			m_fCollisionCooldownTimer += fTimeStep;
		}

		//If reached the position we wanted, return to initial place
		if( UpdateMovement( fTimeStep ) )
		{
			ReverseDirection();
		}
	}
	else
	{
		if( false == m_bAnimationSwitched )
		{
			this->StopAllActions();
			this->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "DeadHarpy" ) ) );
			GetPhysicsBody()->SetLinearVelocity( b2Vec2( 0.0f , 0.0f ) );
			GetPhysicsBody()->SetType( b2_dynamicBody );
			m_bAnimationSwitched = true;
		}
		if( m_fDeathTimer < m_fDeathLimit )
		{
			m_fDeathTimer += fTimeStep;
		}
		else
		{
			CGCObjectManager::ObjectKill( this );
		}
	}

}

bool CGCObjHarpy::UpdateMovement( f32 fTimeStep )
{
	m_v2VelocityDesired = m_v2EndPosition - GetPosition();
	m_v2VelocityDesired.Normalize();
	m_v2LinearVelocity = 230.f * ( fTimeStep * m_v2VelocityDesired );
	GetPhysicsBody()->SetLinearVelocity( m_v2LinearVelocity );

	//Has reached the position we wanted
	if( ( fabs( GetPosition().x - m_v2EndPosition.x ) < 1.0f )
		&& ( fabs( GetPosition().y - m_v2EndPosition.y ) < 1.0f ) )
	{
		m_bFacingDirection = !m_bFacingDirection;
		if( m_v2EndPosition.x != m_v2StartingPosition.x || m_v2EndPosition.y != m_v2StartingPosition.y )
		{
			this->SetFlippedX( m_bFacingDirection );
		}
		return true;
	}
	else
		return false;
}

//reverses the direction of the platform
void CGCObjHarpy::ReverseDirection()
{
	b2Vec2 aux = m_v2EndPosition;
	m_v2EndPosition = m_v2StartingPosition;
	m_v2StartingPosition = aux;
}

void CGCObjHarpy::VHandleFactoryParams( const CGCFactoryCreationParams& rCreationParams, b2Vec2 v2InitialPosition, f32 fInitialRotation )
{
	CGCObjSpritePhysics::VHandleFactoryParams( rCreationParams, v2InitialPosition, fInitialRotation );

	const CGCMovingPlatformFactoryCreationParams* pPlatformParams = static_cast<const CGCMovingPlatformFactoryCreationParams*>( GetFactoryCreationParams() );

	b2Vec2 v2EndPosition = v2InitialPosition;

	if( pPlatformParams->m_fHorizontalEnd != 0.0f )
	{
		v2EndPosition.x += pPlatformParams->m_fHorizontalEnd;
	}

	if( pPlatformParams->m_fVerticalEnd != 0.0f )
	{
		v2EndPosition.y += pPlatformParams->m_fVerticalEnd;
	}

	m_v2StartingPosition = v2InitialPosition;
	m_v2EndPosition = v2EndPosition;

}

void CGCObjHarpy::VOnReset()
{
	CGCObjSpritePhysics::VOnReset();

	this->StopAllActions();
	this->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "IdleHarpy" ) ) );

	m_bDied = false;
	m_fDeathTimer = 0.0f;
	m_bAnimationSwitched = false;

	if( GetPosition().x - m_v2EndPosition.x < 0.f )
	{
		m_bFacingDirection = true;
		this->SetFlippedX( true );
	}
	else
	{
		m_bFacingDirection = false;
		this->SetFlippedX( false );
	}
}