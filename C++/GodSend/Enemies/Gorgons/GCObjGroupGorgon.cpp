////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string.h>

#include "GamerCamp/GameSpecific/Enemies/Gorgons/GCObjGroupGorgon.h"

#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GCCocosInterface/IGCGameLayer.h"
#include "GamerCamp/Core/GCTypes.h"
#include "GamerCamp/GCObject/GCObject.h"
#include "GamerCamp/GameSpecific/Enemies/Gorgons/GCObjGorgon.h"
#include "GamerCamp/GCCocosInterface/GCCocosHelpers.h"

#include "Audio/GCSFXManager.h"

//////////////////////////////////////////////////////////////////////////
// using
using namespace cocos2d;



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CGCObjGroupGorgon::CGCObjGroupGorgon()
{
	m_v2FormationOrigin = b2Vec2_zero;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// virtual
CGCObjGroupGorgon::~CGCObjGroupGorgon()
{
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupGorgon::SetFormationOrigin( b2Vec2 v2FormationOrigin )
{
	m_v2FormationOrigin = v2FormationOrigin;
}

//////////////////////////////////////////////////////////////////////////
// only handle Gorgons
//////////////////////////////////////////////////////////////////////////
//virtual 
bool CGCObjGroupGorgon::VHandlesThisTypeId( GCTypeID idQueryType )
{
	return( GetGCTypeIDOf( CGCObjGorgon ) == idQueryType );
}



//////////////////////////////////////////////////////////////////////////
// must return the typeid of the CGCObjectGroup derived class
//////////////////////////////////////////////////////////////////////////
//virtual 
GCTypeID CGCObjGroupGorgon::VGetTypeId( void )
{
	return GetGCTypeIDOf( CGCObjGroupGorgon );
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//virtual 
// void CGCObjGroupGorgon::VOnGroupResourceAcquire( void )
// {
// 	CreateGorgons();
// 	CGCObjectGroup::VOnGroupResourceAcquire();
// }

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGroupGorgon::VOnGroupResourceAcquire_PostObject( void )
{
	CGCObjectGroup::VOnGroupResourceAcquire_PostObject();

	CGCSFXManager* pSFXManager = CGCSFXManager::GetSharedManager();
	pSFXManager->LoadSoundEffect( AudioFileNames::pszGorgonGazeSound );
	pSFXManager->LoadSoundEffect( AudioFileNames::pszGorgonResetSound );
	pSFXManager->LoadSoundEffect( AudioFileNames::pszGorgonDeathSound );

	// set up animations for all items
	const char* pszPlist_Gorgon = "TexturePacker/Sprites/Common/Enemy_Gorgon/Gorgon.plist";
	const char* pszAnim_Gorgon_Idle = "Gorgon_Idle";
	const char* pszAnim_Gorgon_Attack = "Gorgon_Beam";
	const char* pszAnim_Gorgon_Charge = "Gorgon_Charge";
	const char* pszAnim_Gorgon_Petrified = "Gorgon_Petrified";
	const char* pszAnim_Gorgon_Death = "Gorgon_Death";


	// make an animation
	// N.B. pdictPList is returned autoreleased - will clean itslef at end of frame if not retained
	CCDictionary* pdictPList = GCCocosHelpers::CreateDictionaryFromPlist( pszPlist_Gorgon );
	CCAnimation* pAnimationIdle = GCCocosHelpers::CreateAnimation( pdictPList, pszAnim_Gorgon_Idle );
	CCAnimation* pAnimationCharge = GCCocosHelpers::CreateAnimation( pdictPList, pszAnim_Gorgon_Charge );
	CCAnimation* pAnimationAttack = GCCocosHelpers::CreateAnimation( pdictPList, pszAnim_Gorgon_Attack );
	CCAnimation* pAnimationPetrified = GCCocosHelpers::CreateAnimation( pdictPList, pszAnim_Gorgon_Petrified );

	CCAnimation* pAnimationDeath = GCCocosHelpers::CreateAnimation( pdictPList , pszAnim_Gorgon_Death );

	//loads in sprite frames
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile( pszPlist_Gorgon );

	//add animations to the shared cache for future use(this way only create the animation once)
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationIdle, pszAnim_Gorgon_Idle );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationCharge, pszAnim_Gorgon_Charge );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationAttack, pszAnim_Gorgon_Attack );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationPetrified, pszAnim_Gorgon_Petrified );

	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationDeath , pszAnim_Gorgon_Death );

	CCAnimation* p_StartAnimation = CCAnimationCache::sharedAnimationCache()->animationByName( pszAnim_Gorgon_Idle );

	// N.B. this is a workaround for the fact that Marmalade's version of GCC for ARM doesn't support lambdas. Blergh.
	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each Gorgon Object
	for( u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex )
	{
		CGCObject* pcItemAsObject = sMyGatherer.apObjects[ uIndex ];
		CCAssert( ( GetGCTypeIDOf( CGCObjGorgon ) == pcItemAsObject->GetGCTypeID() ),
				  "CGCObject derived type mismatch!" );

		CGCObjSprite* pItemSprite = ( CGCObjSprite* ) pcItemAsObject;
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
void CGCObjGroupGorgon::VOnGroupResourceRelease( void )
{
	// N.B. need to do this first as it clears internal lists
	CGCObjectGroup::VOnGroupResourceRelease();
	DestroyGorgons();
}

void CGCObjGroupGorgon::VOnGroupUpdate( f32 fTimeStep )
{
	CGCObjectGroup::VOnGroupUpdate(fTimeStep);
	// N.B. this is a workaround for the fact that Marmalade's version of GCC for ARM doesn't support lambdas. Blergh.
	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each Gorgon Object
	for( u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex )
	{
		CGCObject* pcItemAsObject = sMyGatherer.apObjects[ uIndex ];
		CCAssert( ( GetGCTypeIDOf( CGCObjGorgon ) == pcItemAsObject->GetGCTypeID() ),
				  "CGCObject derived type mismatch!" );

		CGCObjGorgon* pItemSprite = ( CGCObjGorgon* ) pcItemAsObject;

		if( pItemSprite->GetDead() && EEmyAniS_death != pItemSprite->GetAnimation() )
		{
			pItemSprite->StopAllActions();
			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "Gorgon_Death" ) ) );
			pItemSprite->SetAnimation( EEmyAniS_death );
			CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszGorgonDeathSound );
		}
		else if( false == pItemSprite->GetDead() )
		{//apply various animation based on gorgon's current behaviour
			if( false == pItemSprite->GetAttacking() && false == pItemSprite->GetCharging() && false == pItemSprite->GetStunned() && EEmyAniS_idle != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "Gorgon_Idle" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_idle );
			}
			else if( pItemSprite->GetAttacking() && EEmyAniS_attack != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "Gorgon_Beam" ) ) );
				CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszGorgonGazeSound );
				pItemSprite->SetAnimation( EEmyAniS_attack );
			}
			else if( pItemSprite->GetCharging() && EEmyAniS_charge != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "Gorgon_Charge" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_charge );
			}
			else if( pItemSprite->GetStunned() && !pItemSprite->GetBreaking() && EEmyAniS_stun != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "Gorgon_Petrified" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_stun );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// void CGCObjGroupGorgon::CreateGorgons( void )
// {
// 
// }



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupGorgon::DestroyGorgons( void )
{
	CGCObjGroupGorgon::SArrayOfGorgons sArray = { 0, { 0 } };
	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each Gorgon Object
	for (u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex)
	{
		sArray.apGorgons[ sArray.uCount++ ] = static_cast< CGCObjGorgon* >( sMyGatherer.apObjects[ uIndex ] );
	}

	for( u32 uLoop = 0; uLoop < sArray.uCount; ++uLoop )
	{
		// destroy each Gorgon's sprite
		CCAssert( ( GetGCTypeIDOf( CGCObjGorgon ) == sArray.apGorgons[ uLoop ]->GetGCTypeID() ),
				  "eek! something very bad has happened!" );
		sArray.apGorgons[ uLoop ]->DestroySprite();
		delete sArray.apGorgons[ uLoop ];
		sArray.apGorgons[ uLoop ] = NULL;
	}
}



//////////////////////////////////////////////////////////////////////////
// static function conforming to PFNGCObjectGroupListVisitor typedef
//////////////////////////////////////////////////////////////////////////
//static 
bool CGCObjGroupGorgon::GroupVisitor_FindByName( void* psFindByName, CGCObject* pcObjToCheck )
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
bool CGCObjGroupGorgon::GroupVisitor_GetAllGorgons( void* psArrayOfGorgons, CGCObject* pcObject )
{
	// gathers all Gorgons into the SArrayOfGorgons passed in via the void*
	CGCObjGroupGorgon::SArrayOfGorgons* psArray = ( CGCObjGroupGorgon::SArrayOfGorgons* ) psArrayOfGorgons;
	psArray->apGorgons[ psArray->uCount++ ] = static_cast< CGCObjGorgon* >( pcObject );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// static function conforming to PFNGCObjectGroupListVisitor typedef
//////////////////////////////////////////////////////////////////////////
//static 
bool CGCObjGroupGorgon::GroupVisitor_GetAllSprites( void* psArrayOfSprites, CGCObject* pcObject )
{
	// gathers all Gorgons into the SArrayOfGorgons passed in via the void*
	CGCObjGroupGorgon::SArrayOfSprites* psArray = ( CGCObjGroupGorgon::SArrayOfSprites* ) psArrayOfSprites;
	psArray->apSprites[ psArray->uCount++ ] = static_cast< CGCObjSprite* >( pcObject );

	return true;
}

