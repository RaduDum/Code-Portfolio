////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <memory.h>

#include "GamerCamp/GCObject/GCObjectManager.h"
#include "GamerCamp/GameSpecific/Player/GCObjProjectilePlayer.h"
#include "GamerCamp/GameSpecific/Player/GCObjGroupProjectilePlayer.h"

#include "GamerCamp/GCCocosInterface/FactoryParameters/GCCyclopsFactoryParams.h"

#include "GCObjCyclops.h"

USING_NS_CC;

GCFACTORY_IMPLEMENT_CREATEABLECLASS( CGCObjCyclops );

//////////////////////////////////////////////////////////////////////////
// GetGCTypeIDOf uses the template in GCTypeID to generate a unique ID for 
// this type - need this to construct our base type
CGCObjCyclops::CGCObjCyclops( void )
	: CGCObjPatrollingEnemy( GetGCTypeIDOf( CGCObjCyclops ) )
{
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGCObject interface



//virtual
void CGCObjCyclops::VHandleFactoryParams( const CGCFactoryCreationParams& rCreationParams, b2Vec2 v2InitialPosition, f32 fInitialRotation )
{
	CGCObjSpritePhysics::VHandleFactoryParams( rCreationParams, v2InitialPosition, fInitialRotation );

	const CGCCyclopsFactoryCreationParams* pCyclopsParams = static_cast< const CGCCyclopsFactoryCreationParams* >( GetFactoryCreationParams() );

	i32 iCyclopsType = pCyclopsParams->m_iCyclopsType;

	switch( iCyclopsType )
	{
	case 0:
		m_bHasShield = false;
		m_bHasHelmet = false;
		break;
	case 1:
		m_bHasHelmet = true;
		m_bHasShield = false;
		break;
	case 2:
		m_bHasHelmet = false;
		m_bHasShield = true;
		break;
	case 3:
		m_bHasHelmet = true;
		m_bHasShield = true;
		break;
	default:
		m_bHasShield = false;
		m_bHasHelmet = false;
		break;
	}

}
