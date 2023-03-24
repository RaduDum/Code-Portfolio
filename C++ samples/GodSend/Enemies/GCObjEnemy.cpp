////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <memory.h>

#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GCCocosInterface/IGCGameLayer.h"

#include "GamerCamp/GCCocosInterface/FactoryParameters/GCEnemyFactoryParams.h"

#include "Gamercamp/GameSpecific/Items/GCObjItem.h"
#include "GCObjEnemy.h"

#include "GamerCamp/GCCocosInterface/GCCocosHelpers.h"

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
// GetGCTypeIDOf uses the template in GCTypeID to generate a unique ID for 
// this type - need this to construct our base type
CGCObjEnemy::CGCObjEnemy( GCTypeID idDerivedType )
	: CGCObjSpritePhysics( idDerivedType )
	, m_bIsDead					( false )
	, m_bIsStunned				( false )
	, m_fDirection				( 1.0f )
	, m_fDeathTimer				( 0.5f )
	, m_fDeathLimit				( 0.5f )
	, m_fStunLimit				( 5.0f )
	, m_fStunTimer				( 5.0f )
	, m_fEnemySwitchTimer		( 1.0f )
	, m_fEnemySwitchLimit		( 1.0f )
	, m_fPushTimer				( 0.0f )
	, m_fPushTimerLimit			( 0.2f )
	, m_fHorizontalPushSpeed	( 5.0f )
	, m_fVerticalPushSpeed		( 5.0f )
	, m_fKnockedBackTimer		( 0.0f )
	, m_fKnockedBackTimerLimit	( 1.0f )
	, m_bHasCollectable			( true )
	, m_bDefaultXFacing			( false )
	, m_cDeathVFX( new CGCObjDashableVFX( "Death Pop", "TexturePacker/Sprites/Common/Enemy_Death_VFX/Enemy_Death_VFX.plist", 15 ) )
	, m_cHitVFX( new CGCObjDashableVFX( "Hit FX", "TexturePacker/Sprites/Common/Enemy_Death_VFX/Enemy_Death_VFX.plist", 15 ) )
{
}


//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjEnemy::VOnResourceAcquire( void )
{
	CGCObjSpritePhysics::VOnResourceAcquire();

	m_pcVFXSprite = new CGCObjSprite();
	m_pcVFXSprite->CreateSprite( "TexturePacker/Sprites/Common/VFX/VFX.plist" );

	m_pcVFXSprite->SetResetPosition( b2Vec2( -100.0f, -100.0f ) );

	m_pcVFXSprite->GetSprite()->setZOrder( 20 );
	m_pcVFXSprite->SetVisible( true );
	m_pcVFXSprite->SetParent( IGCGameLayer::ActiveInstance() );

	if( !CCAnimationCache::sharedAnimationCache()->animationByName( "VFX_Land" ) )
	{
		cocos2d::CCDictionary* pVFXDictList = GCCocosHelpers::CreateDictionaryFromPlist( "TexturePacker/Sprites/Common/VFX/VFX.plist" );
		CCAnimation* pLandVFXAnimation = GCCocosHelpers::CreateAnimation( pVFXDictList, "VFX_Land" );
		CCAnimationCache::sharedAnimationCache()->addAnimation( pLandVFXAnimation, "VFX_Land" );
		CCAnimation* pObstacleVFXAnimation = GCCocosHelpers::CreateAnimation( pVFXDictList, "VFX_Obstacle" );
		CCAnimationCache::sharedAnimationCache()->addAnimation( pObstacleVFXAnimation, "VFX_Obstacle" );
	}

	const CGCEnemyFactoryCreationParams* pEnemyParams = static_cast< const CGCEnemyFactoryCreationParams* >( GetFactoryCreationParams() );

	m_bHasCollectable = pEnemyParams->m_bHasCollectable;

	GetSprite()->setZOrder( 11 );

	//check to see if enemy is holding a collectable
	if( m_bHasCollectable == true )
	{
		//set an offscreen position for the collectable gameobject
		b2Vec2 v2ScrollPos( - 1000.0f, -1000.0f );

		// factory creation parameters
		CGCFactoryCreationParams& sScrollParams = m_sScrollParams;
		sScrollParams.strClassName = "CGCObjItem";
		sScrollParams.strPlistFile = "TexturePacker/Sprites/Common/Collectable_Scroll/Scroll.plist";
		sScrollParams.strPhysicsShape = "scroll";
		sScrollParams.eB2dBody_BodyType = b2_staticBody;
		sScrollParams.bB2dBody_FixedRotation = true;

		//create scroll object
		m_pcScroll = static_cast< CGCObjItem* >( CGCFactory_ObjSpritePhysics::GetFactory().CreateInstance( sScrollParams, v2ScrollPos, 0.0f ) );

		//go through same methods as a standard collectable
		m_pcScroll->VOnResourceAcquire();
		m_pcScroll->VOnResurrected();
		m_pcScroll->SetAwake( false );
		m_pcScroll->SetResetPosition(b2Vec2( -100.0f , -100.0f) );

		//set seperate indicator to show that enemy is carrying collectable
		m_cCollectableBubble = new CGCObjSprite();
		m_cCollectableBubble->CreateSprite( "TexturePacker/Sprites/Common/MailBubble/MailBubble.plist" );

		//load in animation for the cgcobjspriet
		CCDictionary* pdictPList = GCCocosHelpers::CreateDictionaryFromPlist( "TexturePacker/Sprites/Common/MailBubble/MailBubble.plist" );
		CCAnimation* pAnimationFloat = GCCocosHelpers::CreateAnimation( pdictPList , "CollectableBubble" );

		//run animation
		m_cCollectableBubble->GetSprite()->runAction( GCCocosHelpers::CreateAnimationActionLoop( pAnimationFloat ) );

		//set the position offscreen
		m_cCollectableBubble->SetResetPosition( b2Vec2( -100.0f , -100.0f ) );

		m_cCollectableBubble->GetSprite()->setZOrder( 10 );
		m_cCollectableBubble->SetVisible( true );
		m_cCollectableBubble->SetParent( IGCGameLayer::ActiveInstance() );
	}

}

void CGCObjEnemy::VOnResurrected( void )
{
	CGCObjSpritePhysics::VOnResurrected();

	//call the appropriate function for the collectable
	if( m_bHasCollectable == true )
	{
		m_pcScroll->VOnResurrected();
	}
	

	SetPhysicsParameters();
}

void CGCObjEnemy::VOnReset( void )
{
	CGCObjSpritePhysics::VOnReset();

	//reset timer and collision
	m_fStunTimer			= m_fStunLimit;
	m_fDeathTimer			= m_fDeathLimit;
	m_iNumberOfCollisions = 0;

	//reset physics density and frictions
	if (this->GetPhysicsBody() != nullptr)
	{
		
		for (b2Fixture* pFixture = this->GetPhysicsBody()->GetFixtureList();
			pFixture != nullptr;
			pFixture = pFixture->GetNext())
		{
			pFixture->SetDensity( 2.0f );
			pFixture->SetFriction( 1.0f );
		}
	}

	if( true == m_bHasCollectable )
	{
		//reset collectable 
		m_cCollectableBubble->SetVisible( true );

		//reset scroll
		m_pcScroll->VOnReset();
		m_pcScroll->SetAwake( false );
		m_pcScroll->Teleport( b2Vec2( -100.0f , 100.0f ) );

	}

	//reset current enemy direction
	if( m_bDefaultXFacing == false )
	{
		m_fDirection = -1.0f;
	}
	else
	{
		m_fDirection = 1.0f;
	}

	//reset current enemy states
	m_bIsDead = false;
	m_bIsStunned = false;

	m_bIsAttacking = false;

	//reset flip for object
	SetFlippedX( false );
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
//apply physics
void CGCObjEnemy::SetPhysicsParameters( void )
{
	//Set the physics body to not rotate
	if( GetPhysicsBody()->GetGravityScale() == 1.0f )
	{
		//makes the player character fall faster, bundled here with other physics body init code
		GetPhysicsBody()->SetGravityScale( 2.5f );
	}
}



//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjEnemy::VOnUpdate( f32 fTimeStep )
{
	CGCObjSpritePhysics::VOnUpdate( fTimeStep );
	//check for enemy death if true, play vfx, remove enemy from play along wiht collectable
	if( GetDead() )
	{
		m_cDeathVFX->PlayVFX();
		//countdown timer for until the enemy is removed from play
		if( m_fDeathTimer < m_fDeathLimit )
		{
			m_fDeathTimer += fTimeStep;
		}
		//when enemy is removed
		else if( m_fDeathTimer >= m_fDeathLimit )
		{
			//move the scroll on screen after a bit of delay
			if( m_bHasCollectable )
			{
				//teleports the scroll at the last enemy location
				//first we set the position of the scroll in the game 
				m_pcScroll->SetPosition( b2Vec2( this->GetPosition().x ,
					this->GetPosition().y - m_pcScroll->GetSprite()->getContentSize().height / 2 ) );
				
				//apply fade in effect for a less awkward look
				m_pcScroll->GetSprite()->runAction( CCFadeIn::create( 0.25f ) );

				//move the transport using a seperate function in the CGCObjItem class
				m_pcScroll->Teleport( this->GetPosition() );
				
				//hide the bubble indicating the player holding mail
				m_cCollectableBubble->SetVisible( false );
			}
			//remove all vfx
			m_cDeathVFX->KillVFX( true );
			m_cHitVFX->KillVFX( true );
			CGCObjectManager::ObjectKill( this );
			//return once enemy dies
			return;
		}
	}

	//if enemy has collectable, the collectable bubble tracks the enemy's position
	if( true == m_bHasCollectable)
	{
		m_cCollectableBubble->SetPosition( b2Vec2( this->GetPosition().x ,
			this->GetPosition().y + 
			( this->GetSprite()->getContentSize().height * 0.4f ) + 
			m_cCollectableBubble->GetSprite()->getContentSize().height * 0.4f ) );
	}

	//decrement pushing timer so player cannot apply pushing force multiple times
	if( m_fPushTimer < m_fPushTimerLimit )
	{
		m_fPushTimer += fTimeStep;
	}

	//decrement knockback timer so enemy cannot get knockedback multiple times
	if( m_fKnockedBackTimer < m_fKnockedBackTimerLimit )
	{
		m_fKnockedBackTimer += fTimeStep;
	}

	//timer used to keep track of enemy direction and how often he has switched direction
	if( m_fEnemySwitchTimer < m_fEnemySwitchLimit )
	{
		m_fEnemySwitchTimer += fTimeStep;
	}

	//stun timer
	if( m_fStunTimer < m_fStunLimit )
	{
		m_fStunTimer += fTimeStep;
	}
	//check if the stun has expired 
	else if ( m_fStunTimer >= m_fStunLimit && true == m_bIsStunned)
	{
		//reset stun values
		m_bIsStunned = false;
		m_fStunTimer = 5.0f;
		//change enemy direction
		SetFlippedX( !IsFlippedX() );
		m_fDirection = -m_fDirection;
	}

	//the vfx should follow the enemy
	m_cDeathVFX->VOnUpdate( fTimeStep, b2Vec2( this->GetPosition().x, this->GetPosition().y ) );
	m_cHitVFX->VOnUpdate( fTimeStep, b2Vec2( this->GetPosition().x, this->GetPosition().y ) );

}




//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
//retrieve varius behaviour types 
void CGCObjEnemy::VChangeBehaviour( int iCollisionType )
{ 
	switch( iCollisionType )
		{
			//check if enemy has just landed on a platform
			case EEmyBehav_platform_start:
			{
				//apply dust effect if they're currently landing(transitioning from 0 collisions to 1 collision)
				if( 0 == m_iNumberOfCollisions )
				{
					GetVFXSprite()->SetPosition( b2Vec2( GetPosition().x,
						GetPosition().y -
						GetSprite()->getContentSize().height * 0.5f ) );

					GetVFXSprite()->StopAllActions();
					GetVFXSprite()->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "VFX_Land" ) ) );
				}
				m_iNumberOfCollisions++;
				break;
			}
			//stops movement if enemy falls off platform
			case EEmyBehav_platform_end:
			{
				m_iNumberOfCollisions--;
				break;
			}
			//check left direction
			case EEmyBehav_direction_switch_left:
			{
				//check timer
				if( m_fEnemySwitchTimer >= m_fEnemySwitchLimit )
				{
					bool bDirectionSwitched = false;
					//check if enemy is on ground and if theyre facing the opposite way
					if( m_iNumberOfCollisions > 0 && false == IsFlippedX() )
					{
						m_fDirection = 1.0f;
						bDirectionSwitched = true;
						SetFlippedX( true );
					}
					//check if the direction has switched to reset timer
					if( true == bDirectionSwitched )
					{
						m_fEnemySwitchTimer = 0.0f;
					}
				}
				break;
			}
			//check right direction
			case EEmyBehav_direction_switch_right:
			{
				//check timer
				if( m_fEnemySwitchTimer >= m_fEnemySwitchLimit )
				{
					bool bDirectionSwitched = false;
					//check if enemy is on ground and if theyre facing the opposite way
					if( m_iNumberOfCollisions > 0 && true == IsFlippedX() )
					{
						m_fDirection = -1.0f;
						bDirectionSwitched = true;
						SetFlippedX( false );
					}
					//check if the direction has switched to reset timer
					if( true == bDirectionSwitched )
					{
						m_fEnemySwitchTimer = 0.0f;
					}
				}
				break;
			}
			//switch direction to opposite of current
			case EEmyBehav_direction_switch:
			{
				//check timer
				if( m_fEnemySwitchTimer >= m_fEnemySwitchLimit )
				{
					bool bDirectionSwitched = false;
					//check if enemy is on ground and if theyre facing the opposite way
					if( m_iNumberOfCollisions > 0 )
					{
						m_fDirection = -1.0f * m_fDirection;
						bDirectionSwitched = true;
						SetFlippedX( !IsFlippedX() );
					}
					//check if the direction has switched to reset timer
					if( true == bDirectionSwitched )
					{
						m_fEnemySwitchTimer = 0.0f;
					}
				}
				break;
			}
			//stun enemy
			case EEmyBehav_stun:
			{
				ActivateStun();
				break;
			}
			//kill object
			case EEmyBehav_die:
			{
				m_cHitVFX->PlayVFX();
				if( false == GetDead() )
				{
					SetDead( true );
					m_fDeathTimer = 0.0f;
				}
				break;
			}
			default:
			{
				break;
			}
		}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
//set stunned
void CGCObjEnemy::ActivateStun( void )
{
	//checks to see if the enemy is not already in stun state
	// resets stun values
	if( false == m_bIsStunned  )
	{
		m_fStunTimer = 0.0f;
		m_bIsStunned = true;
	}
}
//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
//push when player dashes into
void CGCObjEnemy::Push( float fDirectionValue )
{
	//checks the timer to see if it hasn't just been pushed
  	if (m_fPushTimer >= m_fPushTimerLimit)
	{
		m_fPushTimer = 0.0f;

		//apply forces to the enemy
		f32 fHorizontalImpulse = GetPhysicsBody()->GetMass() * m_fHorizontalPushSpeed * fDirectionValue;
		f32 fVerticalImpulse = GetPhysicsBody()->GetMass() * m_fVerticalPushSpeed;

		//done using linear impulse rather than linear velocity
		GetPhysicsBody()->ApplyLinearImpulse( b2Vec2( fHorizontalImpulse, fVerticalImpulse ), GetPhysicsBody()->GetWorldCenter() );
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjEnemy::ApplyKnockBack( f32 fDirectionValue )
{
	//if the pushable has hit an enemy, it is propelled.
	if (m_fKnockedBackTimer >= m_fKnockedBackTimerLimit)
	{
		m_fKnockedBackTimer = 0.0f;
		b2Vec2 v2CurrentVelocity = GetPhysicsBody()->GetLinearVelocity();
		v2CurrentVelocity.Normalize();

		//apply forces to the enemy
		f32 fHorizontalImpulse = GetPhysicsBody()->GetMass() * m_fHorizontalPushSpeed * fDirectionValue;
		f32 fVerticalImpulse = GetPhysicsBody()->GetMass() * m_fVerticalPushSpeed;

		//done using linear impulse rather than linear velocity
		GetPhysicsBody()->ApplyLinearImpulse( b2Vec2 (fHorizontalImpulse, fVerticalImpulse), GetPhysicsBody()->GetWorldCenter() );

	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjEnemy::VOnResourceRelease()
{
	//on release destroy the collectable as well
	if( true == m_bHasCollectable )
	{
		delete m_cCollectableBubble;
		m_cCollectableBubble = nullptr;
	}
	CGCObjSpritePhysics::VOnResourceRelease();
}
