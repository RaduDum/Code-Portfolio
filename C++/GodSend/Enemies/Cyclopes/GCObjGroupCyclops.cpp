////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string.h>

#include "GamerCamp/GameSpecific/Enemies/Cyclopes/GCObjGroupCyclops.h"

#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GCCocosInterface/IGCGameLayer.h"
#include "GamerCamp/Core/GCTypes.h"
#include "GamerCamp/GCObject/GCObject.h"
#include "GamerCamp/GameSpecific/Enemies/Cyclopes/GCObjCyclops.h"
#include "GamerCamp/GCCocosInterface/GCCocosHelpers.h"

#include "Audio/GCSFXManager.h"

//////////////////////////////////////////////////////////////////////////
// using
using namespace cocos2d;



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CGCObjGroupCyclops::CGCObjGroupCyclops()
{
	m_v2FormationOrigin = b2Vec2_zero;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// virtual
CGCObjGroupCyclops::~CGCObjGroupCyclops()
{
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupCyclops::SetFormationOrigin( b2Vec2 v2FormationOrigin )
{
	m_v2FormationOrigin = v2FormationOrigin;
}

//////////////////////////////////////////////////////////////////////////
// only handle Cyclopes
//////////////////////////////////////////////////////////////////////////
//virtual 
bool CGCObjGroupCyclops::VHandlesThisTypeId( GCTypeID idQueryType )
{
	return( GetGCTypeIDOf( CGCObjCyclops ) == idQueryType );
}



//////////////////////////////////////////////////////////////////////////
// must return the typeid of the CGCObjectGroup derived class
//////////////////////////////////////////////////////////////////////////
//virtual 
GCTypeID CGCObjGroupCyclops::VGetTypeId( void )
{
	return GetGCTypeIDOf( CGCObjGroupCyclops );
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGroupCyclops::VOnGroupResourceAcquire_PostObject( void )
{
	// parent class version
	CGCObjectGroup::VOnGroupResourceAcquire_PostObject();

	CGCSFXManager* pSFXManager = CGCSFXManager::GetSharedManager();
	pSFXManager->LoadSoundEffect( AudioFileNames::pszCyclopsDeathSound );

	//animation plist's
	const char* pszCyclopsPlist = "TexturePacker/Sprites/Common/Enemy_Cyclops/Cyclops.plist";
	const char* pszHCyclopsPlist = "TexturePacker/Sprites/Common/Enemy_Cyclops_H/EnemyHCyclops.plist";
	const char* pszHSCyclopsPlist = "TexturePacker/Sprites/Common/Enemy_Cyclops_HS/EnemyHSCyclops.plist";
	const char* pszSCyclopsPlist = "TexturePacker/Sprites/Common/Enemy_Cyclops_S/EnemySCyclops.plist";

	//individual enemy animation frames
	const char* pszAnim_Cyclops_Idle = "Cyclops_Walk";
	const char* pszAnim_HCyclops_Idle = "EHCyclopsIdle";
	const char* pszAnim_HSCyclops_Idle = "EHSCyclopsIdle";
	const char* pszAnim_SCyclops_Idle = "ESCyclopsIdle";
	const char* pszAnim_Cyclops_Petrified = "Cyclops_Petrified";
	const char* pszAnim_HCyclops_Petrified = "EHCyclopsPetrified";
	const char* pszAnim_HSCyclops_Petrified = "EHSCyclopsPetrified";
	const char* pszAnim_SCyclops_Petrified = "ESCyclopsPetrified";
	const char* pszAnim_Cyclops_Dead = "Cyclops_Dead";
	const char* pszAnim_HCyclops_Dead = "EHCyclopsDead";
	const char* pszAnim_HSCyclops_Dead = "EHSCyclopsDead";
	const char* pszAnim_SCyclops_Dead = "ESCyclopsDead";

	//loading dictionary from the plist
	CCDictionary*	pdictCyclopsPList = GCCocosHelpers::CreateDictionaryFromPlist( pszCyclopsPlist );
	CCDictionary*	pdictHCyclopsPList = GCCocosHelpers::CreateDictionaryFromPlist( pszHCyclopsPlist );
	CCDictionary*	pdictHSCyclopsPList = GCCocosHelpers::CreateDictionaryFromPlist( pszHSCyclopsPlist );
	CCDictionary*	pdictSCyclopsPList = GCCocosHelpers::CreateDictionaryFromPlist( pszSCyclopsPlist );

	//create animations
	CCAnimation*	pAnimationIdle = GCCocosHelpers::CreateAnimation( pdictCyclopsPList, pszAnim_Cyclops_Idle );
	CCAnimation*	pAnimationHIdle = GCCocosHelpers::CreateAnimation( pdictHCyclopsPList, pszAnim_HCyclops_Idle );
	CCAnimation*	pAnimationHSIdle = GCCocosHelpers::CreateAnimation( pdictHSCyclopsPList, pszAnim_HSCyclops_Idle );
	CCAnimation*	pAnimationSIdle = GCCocosHelpers::CreateAnimation( pdictSCyclopsPList, pszAnim_SCyclops_Idle );
	CCAnimation*	pAnimationPetrified = GCCocosHelpers::CreateAnimation( pdictCyclopsPList, pszAnim_Cyclops_Petrified );
	CCAnimation*	pAnimationHPetrified = GCCocosHelpers::CreateAnimation( pdictHCyclopsPList, pszAnim_HCyclops_Petrified );
	CCAnimation*	pAnimationHSPetrified = GCCocosHelpers::CreateAnimation( pdictHSCyclopsPList, pszAnim_HSCyclops_Petrified );
	CCAnimation*	pAnimationSPetrified = GCCocosHelpers::CreateAnimation( pdictSCyclopsPList , pszAnim_SCyclops_Petrified );
	CCAnimation*	pAnimationDead = GCCocosHelpers::CreateAnimation( pdictCyclopsPList , pszAnim_Cyclops_Dead );
	CCAnimation*	pAnimationHDead = GCCocosHelpers::CreateAnimation( pdictHCyclopsPList , pszAnim_HCyclops_Dead );
	CCAnimation*	pAnimationHSDead = GCCocosHelpers::CreateAnimation( pdictHSCyclopsPList , pszAnim_HSCyclops_Dead );
	CCAnimation*	pAnimationSDead = GCCocosHelpers::CreateAnimation( pdictSCyclopsPList , pszAnim_SCyclops_Dead );


	//loads in sprite frames
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationIdle, pszAnim_Cyclops_Idle );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationHIdle, pszAnim_HCyclops_Idle );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationHSIdle, pszAnim_HSCyclops_Idle );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationSIdle, pszAnim_SCyclops_Idle );

	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationPetrified, pszAnim_Cyclops_Petrified );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationHPetrified, pszAnim_HCyclops_Petrified );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationHSPetrified, pszAnim_HSCyclops_Petrified );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationSPetrified, pszAnim_SCyclops_Petrified );

	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationDead , pszAnim_Cyclops_Dead );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationHDead , pszAnim_HCyclops_Dead );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationHSDead , pszAnim_HSCyclops_Dead );
	CCAnimationCache::sharedAnimationCache()->addAnimation( pAnimationSDead , pszAnim_SCyclops_Dead );

	CGCObjectGroup::VOnGroupResourceAcquire_PostObject();
	CCAnimation* p_StartAnimation = CCAnimationCache::sharedAnimationCache()->animationByName( pszAnim_Cyclops_Idle );
	CCAnimation* p_StartHAnimation = CCAnimationCache::sharedAnimationCache()->animationByName( pszAnim_HCyclops_Idle );
	CCAnimation* p_StartHSAnimation = CCAnimationCache::sharedAnimationCache()->animationByName( pszAnim_HSCyclops_Idle );
	CCAnimation* p_StartSAnimation = CCAnimationCache::sharedAnimationCache()->animationByName( pszAnim_SCyclops_Idle );

	// N.B. this is a workaround for the fact that Marmalade's version of GCC for ARM doesn't support lambdas. Blergh.
	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each Cyclops Object
	for (u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex)
	{
		CGCObject* pcItemAsObject = sMyGatherer.apObjects[ uIndex ];
		CCAssert( ( GetGCTypeIDOf( CGCObjCyclops ) == pcItemAsObject->GetGCTypeID() ),
			"CGCObject derived type mismatch!" );

		CGCObjCyclops* pItemSprite = ( CGCObjCyclops* ) pcItemAsObject;
		//apply starting animation for each cyclops type
		if( false == pItemSprite->HasHelmet() && false == pItemSprite->HasShield() )
		{
			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( p_StartAnimation ) );
		}
		else if( true == pItemSprite->HasHelmet() && false == pItemSprite->HasShield() )
		{
			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( p_StartHAnimation ) );
		}
		else if( true == pItemSprite->HasHelmet() && true == pItemSprite->HasShield() )
		{
			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( p_StartHSAnimation ) );
		}
		else if( false == pItemSprite->HasHelmet() && true == pItemSprite->HasShield() )
		{
			pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( p_StartSAnimation ) );
		}

	}

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//virtual 
void CGCObjGroupCyclops::VOnGroupResourceRelease( void )
{
	// N.B. need to do this first as it clears internal lists
	CGCObjectGroup::VOnGroupResourceRelease();
	DestroyCyclopes();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupCyclops::VOnGroupUpdate( f32 fTimeStep )
{

	CGCObjectGroup::VOnGroupUpdate( fTimeStep );

	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each cyclops Object
	for (u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex)
	{
		CGCObject* pcItemAsObject = sMyGatherer.apObjects[ uIndex ];
		CCAssert( ( GetGCTypeIDOf( CGCObjCyclops ) == pcItemAsObject->GetGCTypeID() ),
			"CGCObject derived type mismatch!" );

		CGCObjCyclops* pItemSprite = ( CGCObjCyclops* ) pcItemAsObject;

		//apply various animation based on cyclops's current behaviour
		if (false == pItemSprite->HasHelmet() && false == pItemSprite->HasShield())
		{
			//apply death animation for standard
			if( true == pItemSprite->GetDead() && EEmyAniS_death != pItemSprite->GetAnimation() )
			{
				//stop all actions and apply new animation
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "Cyclops_Dead" ) ) );
				// play sound effect
				CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszCyclopsDeathSound );
				// change the enemy's current animation state in order to keep track of what animations can be loaded in next
				pItemSprite->SetAnimation( EEmyAniS_death );
			}
			//apply walk animation for standard
			else if( false == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() &&EEmyAniS_walk != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "Cyclops_Walk" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_walk );
			}
			//apply petrified animation for standard
			else if( true == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() && EEmyAniS_petrified != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "Cyclops_Petrified" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_petrified );
			}
		}
		//apply animations for helmet cyclops
		else if( true == pItemSprite->HasHelmet() && false == pItemSprite->HasShield() )
		{
			//apply death animation for helmet cyclops
			if( true == pItemSprite->GetDead() && EEmyAniS_death != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "EHCyclopsDead" ) ) );
				CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszCyclopsDeathSound );
				pItemSprite->SetAnimation( EEmyAniS_death );
			}
			//apply walk animation for helmet cyclops
			else if( false == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() && EEmyAniS_walk != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "EHCyclopsIdle" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_walk );
			}
			//apply petrified animation for helmet cyclops
			else if( true == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() && EEmyAniS_petrified != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "EHCyclopsPetrified" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_petrified );
			}
		}
		//apply animation for helmet shiedl cyclops
		else if( true == pItemSprite->HasHelmet() && true == pItemSprite->HasShield() )
		{
			//apply death animation for helmet and shield cyclops
			if( true == pItemSprite->GetDead() && EEmyAniS_death != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "EHSCyclopsDead" ) ) );
				CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszCyclopsDeathSound );
				pItemSprite->SetAnimation( EEmyAniS_death );
			}
			//apply walk animation for helmet and shield cyclops
			else if( false == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() && EEmyAniS_walk != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "EHSCyclopsIdle" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_walk );
			}
			//apply petrified animation for helmet and shield cyclops
			else if( true == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() && EEmyAniS_petrified != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "EHSCyclopsPetrified" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_petrified );
			}
		}
		//apply animations for shield cyclops
		else if( false == pItemSprite->HasHelmet() && true == pItemSprite->HasShield() )
		{
			//apply death animation for  shield cyclops
			if( true == pItemSprite->GetDead() && EEmyAniS_death != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "ESCyclopsDead" ) ) );
				CGCSFXManager::GetSharedManager()->PlaySoundEffect( AudioFileNames::pszCyclopsDeathSound );
				pItemSprite->SetAnimation( EEmyAniS_death );
			}
			//applywalk animation for  shield cyclops
			else if( false == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() && EEmyAniS_walk != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionLoop( CCAnimationCache::sharedAnimationCache()->animationByName( "ESCyclopsIdle" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_walk );
			}
			//apply petrified animation for  shield cyclops
			else if( true == pItemSprite->IsPetrified() && false == pItemSprite->GetDead() && EEmyAniS_petrified != pItemSprite->GetAnimation() )
			{
				pItemSprite->StopAllActions();
				pItemSprite->RunAction( GCCocosHelpers::CreateAnimationActionOnce( CCAnimationCache::sharedAnimationCache()->animationByName( "ESCyclopsPetrified" ) ) );
				pItemSprite->SetAnimation( EEmyAniS_petrified );
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGCObjGroupCyclops::DestroyCyclopes( void )
{
 	CGCObjGroupCyclops::SArrayOfCyclopes sArray = { 0, { 0 } };

	SGCObjectGatherer sMyGatherer;
	ForEachObject( sMyGatherer );

	// create an animation action and set it for each Gorgon Object
	for( u32 uIndex = 0; uIndex < sMyGatherer.uCount; ++uIndex )
	{
		sArray.apCyclopes[ sArray.uCount++ ] = static_cast< CGCObjCyclops* >( sMyGatherer.apObjects[ uIndex ] );
	}

 	for( u32 uLoop = 0; uLoop < sArray.uCount; ++uLoop )
 	{
 		// destroy each Cyclops's sprite
 		CCAssert( ( GetGCTypeIDOf( CGCObjCyclops ) == sArray.apCyclopes[ uLoop ]->GetGCTypeID() ),
				  "eek! something very bad has happened!" );
 		sArray.apCyclopes[ uLoop ]->DestroySprite();
 		delete sArray.apCyclopes[ uLoop ];
 		sArray.apCyclopes[ uLoop ] = NULL;
 	}
}



//////////////////////////////////////////////////////////////////////////
// static function conforming to PFNGCObjectGroupListVisitor typedef
//////////////////////////////////////////////////////////////////////////
//static 
bool CGCObjGroupCyclops::GroupVisitor_FindByName( void* psFindByName, CGCObject* pcObjToCheck )
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
bool CGCObjGroupCyclops::GroupVisitor_GetAllCyclopes( void* psArrayOfCyclopes, CGCObject* pcObject )
{
	// gathers all Cyclopes into the SArrayOfCyclopes passed in via the void*
	CGCObjGroupCyclops::SArrayOfCyclopes* psArray	= ( CGCObjGroupCyclops::SArrayOfCyclopes* ) psArrayOfCyclopes;
	psArray->apCyclopes[ psArray->uCount++ ]		= static_cast< CGCObjCyclops* >( pcObject );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// static function conforming to PFNGCObjectGroupListVisitor typedef
//////////////////////////////////////////////////////////////////////////
//static 
bool CGCObjGroupCyclops::GroupVisitor_GetAllSprites( void* psArrayOfSprites, CGCObject* pcObject )
{
	// gathers all Cyclopes into the SArrayOfCyclopes passed in via the void*
	CGCObjGroupCyclops::SArrayOfSprites* psArray	= ( CGCObjGroupCyclops::SArrayOfSprites* ) psArrayOfSprites;
	psArray->apSprites[ psArray->uCount++ ]			= static_cast< CGCObjSprite* >( pcObject );

	return true;
}

