////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef	_GCOBJECTGROUPCYCLOPS_H_
#define	_GCOBJECTGROUPCYCLOPS_H_


#ifndef BOX2D_H
#include "Box2d/Box2D.h"
#endif

#ifndef	_GCOBJECTGROUP_H_
#include "GamerCamp/GCObject/GCObjectGroup.h"
#endif


//////////////////////////////////////////////////////////////////////////
// forward declare
class CGCObjSprite;
class CGCObjCyclops;


//////////////////////////////////////////////////////////////////////////
// responsible for newing, managing, & deleting the Cyclopes
//
// This shows how an object group can be used as an allocation pool.
//
//////////////////////////////////////////////////////////////////////////
class CGCObjGroupCyclops
	: public CGCObjectGroup
{
private:
	b2Vec2 m_v2FormationOrigin; // origin of the formation

	//void	CreateCyclopes( void );
	void	DestroyCyclopes( void );

public:
	// number of Cyclopes
	static const u32 k_uNumCyclopes = 1;

	CGCObjGroupCyclops();
	virtual ~CGCObjGroupCyclops() override;

	void SetFormationOrigin( b2Vec2 m_v2FormationOrigin );

	//////////////////////////////////////////////////////////////////////////
	// overrides for CGCObjectGroup public interface

	// handles GCObjCyclops
	virtual bool		VHandlesThisTypeId( GCTypeID idQueryType ) override;

	// must return the typeid of the CGCObjectGroup derived class
	virtual GCTypeID	VGetTypeId( void ) override;

	//virtual void		VOnGroupResourceAcquire( void ) override;
	virtual void		VOnGroupResourceAcquire_PostObject( void ) override;
	virtual void		VOnGroupResourceRelease( void ) override;
	virtual void		VOnGroupUpdate( f32 fTimeStep ) override;

	// CGCObjectGroup public interface
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// N.B. this is just to demo how you use the protected visitor interface to CGCObjGroup

	// struct and function used to find an object by name using group visitor pattern
	struct SFindByName
	{
		const char* pstrArgName;
		CGCObject*	pRetObject;
	};
	// static function conforming to PFNGCObjectGroupListVisitor typedef
	static bool GroupVisitor_FindByName( void* psFindByName, CGCObject* pcObjToCheck );
	// N.B. this is just to demo how you use the protected visitor interface to CGCObjGroup
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// used to get all contained objects so they can be deleted
	struct SArrayOfCyclopes
	{
		u32 uCount;
		CGCObjCyclops* apCyclopes[ CGCObjectGroup::EMaxGCObjects ];
	};
	// this function iterates the list filling in an SArrayOfCyclopes passed as the void*
	static bool GroupVisitor_GetAllCyclopes( void* psArrayOfCyclopes, CGCObject* pcObject );

	struct SArrayOfSprites
	{
		u32 uCount;
		CGCObjSprite* apSprites[ CGCObjectGroup::EMaxGCObjects ];
	};
	// this function iterates the list filling in an SArrayOfCyclopesAsSprites passed as the void*
	static bool GroupVisitor_GetAllSprites( void* psArrayOfSprites, CGCObject* pcObject );

	// used to get all contained objects so they can be deleted
	//////////////////////////////////////////////////////////////////////////
};

#endif
