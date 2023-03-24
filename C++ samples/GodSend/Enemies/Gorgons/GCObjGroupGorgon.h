////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef	_GCOBJECTGROUPGORGON_H_
#define	_GCOBJECTGROUPGORGON_H_


#ifndef BOX2D_H
#include "Box2d/Box2D.h"
#endif

#ifndef	_GCOBJECTGROUP_H_
#include "GamerCamp/GCObject/GCObjectGroup.h"
#endif


//////////////////////////////////////////////////////////////////////////
// forward declare
class CGCObjSprite;
class CGCObjGorgon;


//////////////////////////////////////////////////////////////////////////
// responsible for newing, managing, & deleting the Gorgons
//
// This shows how an object group can be used as an allocation pool.
//
//////////////////////////////////////////////////////////////////////////
class CGCObjGroupGorgon
	: public CGCObjectGroup
{
private:
	b2Vec2 m_v2FormationOrigin; // origin of the formation

	//void	CreateGorgons( void );
	void	DestroyGorgons( void );

public:
	// number of Gorgons
	static const u32 k_uNumGorgons = 1;

	CGCObjGroupGorgon();
	virtual ~CGCObjGroupGorgon() override;

	void SetFormationOrigin( b2Vec2 m_v2FormationOrigin );

	//////////////////////////////////////////////////////////////////////////
	// overrides for CGCObjectGroup public interface

	// handles GCObjGorgon
	virtual bool		VHandlesThisTypeId( GCTypeID idQueryType ) override;

	// must return the typeid of the CGCObjectGroup derived class
	virtual GCTypeID	VGetTypeId( void ) override;

	//virtual void		VOnGroupResourceAcquire( void ) override;
	virtual void		VOnGroupResourceAcquire_PostObject( void ) override;
	virtual void		VOnGroupResourceRelease( void ) override;
	virtual	void		VOnGroupUpdate( f32 fTimeStep ) override;

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
	static bool GroupVisitor_FindByName( void* psFindByName , CGCObject* pcObjToCheck );
	// N.B. this is just to demo how you use the protected visitor interface to CGCObjGroup
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// used to get all contained objects so they can be deleted
	struct SArrayOfGorgons
	{
		u32 uCount;
		CGCObjGorgon* apGorgons[ CGCObjectGroup::EMaxGCObjects ];
	};
	// this function iterates the list filling in an SArrayOfGorgons passed as the void*
	static bool GroupVisitor_GetAllGorgons( void* psArrayOfGorgons , CGCObject* pcObject );

	struct SArrayOfSprites
	{
		u32 uCount;
		CGCObjSprite* apSprites[ CGCObjectGroup::EMaxGCObjects ];
	};
	// this function iterates the list filling in an SArrayOfGorgonsAsSprites passed as the void*
	static bool GroupVisitor_GetAllSprites( void* psArrayOfSprites , CGCObject* pcObject );

	// used to get all contained objects so they can be deleted
	//////////////////////////////////////////////////////////////////////////
};

#endif
