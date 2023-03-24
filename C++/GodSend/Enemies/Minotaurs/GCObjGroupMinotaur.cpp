////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string.h>

#include "GamerCamp/GameSpecific/Enemies/Minotaurs/GCObjGroupMinotaur.h"

#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GCCocosInterface/IGCGameLayer.h"
#include "GamerCamp/Core/GCTypes.h"
#include "GamerCamp/GCObject/GCObject.h"
#include "GamerCamp/GameSpecific/Enemies/Minotaurs/GCObjMinotaur.h"
#include "GamerCamp/GCCocosInterface/GCCocosHelpers.h"

#include "Audio/GCSFXManager.h"


//////////////////////////////////////////////////////////////////////////
// using
using namespace cocos2d;



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CGCObjGroupMinotaur::CGCObjGroupMinotaur()
{
	m_v2FormationOrigin = b2Vec2_zero;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// virtual
CGCObjGroupMinotaur::~CGCObjGroupMinotaur()
{
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupMinotaur::SetFormationOrigin( b2Vec2 v2FormationOrigin )
{
	m_v2FormationOrigin = v2FormationOrigin;
}

//////////////////////////////////////////////////////////////////////////
// only handle Minotaurs
//////////////////////////////////////////////////////////////////////////
//virtual 
bool CGCObjGroupMinotaur::VHandlesThisTypeId( GCTypeID idQueryType )
{
	return( GetGCTypeIDOf( CGCObjMinotaur ) == idQueryType );
}



//////////////////////////////////////////////////////////////////////////
// must return the typeid of the CGCObjectGroup derived class
//////////////////////////////////////////////////////////////////////////
//virtual 
GCTypeID CGCObjGroupMinotaur::VGetTypeId( void )
{
	return GetGCTypeIDOf( CGCObjGroupMinotaur );
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGroupMinotaur::VOnGroupResourceAcquire_PostObject( void )
{
	// parent class version
	CGCObjectGroup::VOnGroupResourceAcquire_PostObject();

	CGCSFXManager* pSFXManager = CGCSFXManager::GetSharedManager();
	pSFXManager->LoadSoundEffect( AudioFileNames::pszMinotaurChargeSound );
	pSFXManager->LoadSoundEffect( AudioFileNames::pszMinotaurStunned );
	pSFXManager->LoadSoundEffect( AudioFileNames::pszMinotaurDeathSound );

	// set up animations for all items
	const char* pszPlist	= "TexturePacker/Sprites/Common/Enemy_Minotaur/Minotaur.plist";
	const char* pszAnim_Minotaur_Stunned = "Minotaur_Stunned";
	const char* pszAnim_Minotaur_Walk = "Minotaur_Walk";
	const char* pszAnim_Minotaur_Attack = "Minotaur_Attack";
	const char* pszAnim_Minotaur_Petrified = "Minotaur_Petrified";
	const char* pszAnim_Minotaur_Death = "Minotaur_Death";

	// make an animation
	// N.B. pdictPList is returned autoreleased - will clean itslef at end of frame if not retained
	CCDictionary*	pdictPList = GCCocosHelpers::CreateDictionaryFromPlist( pszPlist );
	CCAnimation*	pAnimationStunned = GCCocosHelpers::CreateAnimation( pdictPList , pszAnim_Minotaur_Stunned );
	CCAnimation*	pAnimationWalk = GCCocosHelpers::CreateAnimation( pdictPList, pszAnim_Minotaur_Walk );
	CCAnimation*	pAnimationAttack = GCCocosHelpers::CreateAnimation( pdictPList, pszAnim_Minotaur_Attack );
	CCAnimation*	pAnimationPetrified = GCCocosHelpers::CreateAnimation( pdictPList , pszAnim_Minotaur_Petrified );
	CCAnimation*	pAnimationDeath = GCCocosHelpers::CreateAnimation( pdictPList , pszAnim_Minotaur_Death );

	//loads in sprite frames
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile( pszPlist );

	//add animations to the shared cache for future use(this way only create the animation once)
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationStunned , pszAnim_Minotaur_Stunned );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationWalk, pszAnim_Minotaur_Walk );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationAttack , pszAnim_Minotaur_Attack );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationPetrified , pszAnim_Minotaur_Petrified );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationDeath , pszAnim_Minotaur_Death );

	CCAnimation* p_StartAnimation = CCAnimationCache::sharedAnimationCache()->animationByName( pszAnim_Minotaur_Walk );

	// N.B. this is a workaround for the fact that Marmalade's version of GCC for ARM doesn't support lambdas. Blergh.
	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each Gorgon Object
	for( u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex )
	{
		CGCObject* pcItemAsObject = sMyGatherer.apObjects[ uIndex ];
		CCAssert( ( GetGCTypeIDOf( CGCObjMinotaur ) == pcItemAsObject->GetGCTypeID() ) ,
			"CGCObject derived type mismatch!" );

		CGCObjSprite* pItemSprite = ( CGCObjSprite* )pcItemAsObject;
		if( pItemSprite->GetName().compare( "Gorgon_Collided" ) != 0 )
		{

			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( p_StartAnimation ) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGroupMinotaur::VOnGroupUpdate( f32 fTimeStep )
{

	CGCObjectGroup::VOnGroupUpdate(fTimeStep);
	// N.B. this is a workaround for the fact that Marmalade's version of GCC for ARM doesn't support lambdas. Blergh.
	SGCObjectGatherer sMyGatherer;
	ForEachObjectIn_LiveList( sMyGatherer );

	// create an animation action and set it for each Gorgon Object
	for (u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex)
	{
		CGCObject* pcItemAsObject = sMyGatherer.apObjects[ uIndex ];
		CCAssert( ( GetGCTypeIDOf( CGCObjMinotaur ) == pcItemAsObject->GetGCTypeID() ),
			"CGCObject derived type mismatch!" );

		CGCObjMinotaur* pItemSprite = ( CGCObjMinotaur* ) pcItemAsObject;

	

		//apply various animation based on gorgon's current behaviour
		if( true == pItemSprite->GetDead() && EEmyAniS_death != pItemSprite->GetAnimation() )
		{
			pItemSprite->StopAllActions();
			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "Minotaur_Death" ) ) );
			CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszMinotaurDeathSound );
			pItemSprite->SetAnimation( EEmyAniS_death );
		}
		else 
			if( true == pItemSprite->GetStunned() && false == pItemSprite->GetDead() && false == pItemSprite->IsPetrified() && EEmyAniS_stun != pItemSprite->GetAnimation() )
		{
			pItemSprite->StopAllActions();
			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "Minotaur_Stunned" ) ) );
			CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszMinotaurStunned );
			pItemSprite->SetAnimation( EEmyAniS_stun );
		}
		else
		{
			if( false == pItemSprite->IsAttacking() && false == pItemSprite->IsPetrified() && false == pItemSprite->GetStunned() && EEmyAniS_walk != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "Minotaur_Walk" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_walk );
			}
			else if( true == pItemSprite->IsAttacking() && false == pItemSprite->IsPetrified() && false == pItemSprite->GetStunned() && EEmyAniS_attack != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "Minotaur_Attack" ) ) );
				CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszMinotaurChargeSound );
				pItemSprite->SetAnimation( EEmyAniS_attack );
			}
			else if( false == pItemSprite->IsAttacking() && true == pItemSprite->IsPetrified() && false == pItemSprite->GetStunned() && EEmyAniS_petrified != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "Minotaur_Petrified" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_petrified );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGroupMinotaur::VOnGroupResourceRelease( void )
{
	// N.B. need to do this first as it clears internal lists
	CGCObjectGroup::VOnGroupResourceRelease();
	DestroyMinotaurs();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupMinotaur::CreateMinotaurs( void )
{
// 	const char* pszPlist_KooperTrooper = "TexturePacker/Sprites/KoopaTrooper/KoopaTrooper.plist";
// 	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile( pszPlist_KooperTrooper );
// 
// 	// N.B. this dictionary is NOT autoreleased and we must remember to release it or it will leak
// 	i32	iOffsetX = 40;
// 	i32	iOffsetY = 100;
// 	for( u32 uLoop = 0; uLoop < k_uNumMinotaurs; ++uLoop )
// 	{
// 		// n.b. these register themselves with this class on creation
// 		CGCObjMinotaur* pMinotaur = new CGCObjMinotaur();
// 		pMinotaur->SetResetPosition( b2Vec2( m_v2FormationOrigin.x + ( iOffsetX * uLoop ), ( m_v2FormationOrigin.y + iOffsetY ) ) );
// 		pMinotaur->SetName( "Derek" );
// 	}
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupMinotaur::DestroyMinotaurs( void )
{
	CGCObjGroupMinotaur::SArrayOfMinotaurs sArray = { 0, { 0 } };
	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each Gorgon Object
	for (u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex)
	{
		sArray.apMinotaurs[ sArray.uCount++ ] = static_cast< CGCObjMinotaur* >( sMyGatherer.apObjects[ uIndex ] );
	}



	//CCAssert( 0 != sArray.uCount, "CGCObjGroupMinotaur::DestroyMinotaurs - failed to find Minotaurs in CGCObjGroupMinotaur!" );

	for( u32 uLoop = 0; uLoop < sArray.uCount; ++uLoop )
	{
		// destroy each Minotaur's sprite
		CCAssert( ( GetGCTypeIDOf( CGCObjMinotaur ) == sArray.apMinotaurs[ uLoop ]->GetGCTypeID() ),
				  "eek! something very bad has happened!" );
		sArray.apMinotaurs[ uLoop ]->DestroySprite();
		delete sArray.apMinotaurs[ uLoop ];
		sArray.apMinotaurs[ uLoop ] = NULL;
	}
}



//////////////////////////////////////////////////////////////////////////
// static function conforming to PFNGCObjectGroupListVisitor typedef
//////////////////////////////////////////////////////////////////////////
//static 
bool CGCObjGroupMinotaur::GroupVisitor_FindByName( void* psFindByName, CGCObject* pcObjToCheck )
{
	// we know the type of psFindByName, because we passed it in in above fn
	SFindByName* pFindByName = reinterpret_cast< SFindByName* >( psFindByName );

	// if there is no name getName will return 0
	const cocos2d::CCString& rstrName = pcObjToCheck->GetName();
	if( 0 == rstrName.compare( pFindByName->pstrArgName ) )
	{
		pFindByName->pRetObject = pcObjToCheck;
		return false;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////
// static function conforming to PFNGCObjectGroupListVisitor typedef
//////////////////////////////////////////////////////////////////////////
//static 
bool CGCObjGroupMinotaur::GroupVisitor_GetAllMinotaurs( void* psArrayOfMinotaurs, CGCObject* pcObject )
{
	// gathers all Minotaurs into the SArrayOfMinotaurs passed in via the void*
	CGCObjGroupMinotaur::SArrayOfMinotaurs* psArray	= ( CGCObjGroupMinotaur::SArrayOfMinotaurs* ) psArrayOfMinotaurs;
	psArray->apMinotaurs[ psArray->uCount++ ]	 = static_cast< CGCObjMinotaur* >( pcObject );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// static function conforming to PFNGCObjectGroupListVisitor typedef
//////////////////////////////////////////////////////////////////////////
//static 
bool CGCObjGroupMinotaur::GroupVisitor_GetAllSprites( void* psArrayOfSprites, CGCObject* pcObject )
{
	// gathers all Minotaurs into the SArrayOfMinotaurs passed in via the void*
	CGCObjGroupMinotaur::SArrayOfSprites* psArray = ( CGCObjGroupMinotaur::SArrayOfSprites* ) psArrayOfSprites;
	psArray->apSprites[ psArray->uCount++ ] = static_cast< CGCObjSprite* >( pcObject );

	return true;
}

