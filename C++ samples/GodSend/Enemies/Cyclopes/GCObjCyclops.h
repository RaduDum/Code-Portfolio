////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GCOBJCYCLOPS_H_
#define _GCOBJCYCLOPS_H_


#include "../../../GCCocosInterface/GCFactory_ObjSpritePhysics.h"

#ifndef _GCOBJPATROLLINGENEMY_H_
#include "GamerCamp/GameSpecific/Enemies/GCObjPatrollingEnemy.h"
#endif

#ifndef	_GCOBJENEMYBEHAVIOURS_H_
#include "GamerCamp/GameSpecific/Minotaurs/GCObjEnemyBehaviours.h"
#endif




//////////////////////////////////////////////////////////////////////////
// This is a sample class derived from CGCObject.
// 
// It could be the basis of your Cyclopsobject, it's up to you really.
//
//////////////////////////////////////////////////////////////////////////
class CGCObjCyclops
	: public CGCObjPatrollingEnemy
{
private:
	//name of sprite for the cyclops
	char* m_pszList;

	//indicator if the cyclops has a shield
	bool m_bHasShield;

	//indicator if the cyclops has a helmet
	bool m_bHasHelmet;

public:
	GCFACTORY_DECLARE_CREATABLECLASS( CGCObjCyclops );

	CGCObjCyclops( void );
	//////////////////////////////////////////////////////////////////////////
	// we need a virtual destructor since delete will be called on pointers of 
	// this class to delete derived types.
	virtual ~CGCObjCyclops() {};

	//////////////////////////////////////////////////////////////////////////
	// overridden virtuals from the game object interface

	virtual void VHandleFactoryParams( const CGCFactoryCreationParams& rCreationParams, b2Vec2 v2InitialPosition, f32 fInitialRotation ) override;

	//accessors
	inline bool HasShield() { return m_bHasShield; }
	inline bool HasHelmet() { return m_bHasHelmet; }
};
#endif // #ifndef _GCOBJCYCLOPS_H_
