////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012 
// This document should not be distributed or reproduced in part or in whole without obtaining written 
// permission from the copyright holders.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef	_GCOBJECTGROUPMINOTAUR_H_
#define	_GCOBJECTGROUPMINOTAUR_H_


#ifndef BOX2D_H
#include "Box2d/Box2D.h"
#endif

#ifndef	_GCOBJECTGROUP_H_
#include "GamerCamp/GCObject/GCObjectGroup.h"
#endif


//////////////////////////////////////////////////////////////////////////
// forward declare
class CGCObjSprite;
class CGCObjMinotaur;


//////////////////////////////////////////////////////////////////////////
// responsible for newing, managing, & deleting the Minotaurs
//
// This shows how an object group can be used as an allocation pool.
//
//////////////////////////////////////////////////////////////////////////
class CGCObjGroupMinotaur
	: public CGCObjectGroup
{
private:
	b2Vec2 m_v2FormationOrigin; // origin of the formation

	void	CreateMinotaurs( void );
	void	DestroyMinotaurs( void );

public:
	// number of Minotaurs
	static const u32 k_uNumMinotaurs = 10;

	CGCObjGroupMinotaur();
	virtual ~CGCObjGroupMinotaur() override;

	void SetFormationOrigin( b2Vec2 m_v2FormationOrigin );

	//////////////////////////////////////////////////////////////////////////
	// overrides for CGCObjectGroup public interface

	// handles GCObjMinotaur
	virtual bool		VHandlesThisTypeId						( GCTypeID idQueryType ) override;

	// must return the typeid of the CGCObjectGroup derived class
	virtual GCTypeID	VGetTypeId								( void ) override;

	virtual	void		VOnGroupUpdate( f32 fTimeStep ) override;
	//virtual void		VOnGroupResourceAcquire					( void ) override;
	virtual void		VOnGroupResourceAcquire_PostObject		( void ) override;
	virtual void		VOnGroupResourceRelease					( void ) override;

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
	struct SArrayOfMinotaurs
	{
		u32 uCount;
		CGCObjMinotaur* apMinotaurs[ CGCObjectGroup::EMaxGCObjects ];
	};
	// this function iterates the list filling in an SArrayOfMinotaurs passed as the void*
	static bool GroupVisitor_GetAllMinotaurs( void* psArrayOfMinotaurs, CGCObject* pcObject );

	struct SArrayOfSprites
	{
		u32 uCount;
		CGCObjSprite* apSprites[ CGCObjectGroup::EMaxGCObjects ];
	};
	// this function iterates the list filling in an SArrayOfMinotaursAsSprites passed as the void*
	static bool GroupVisitor_GetAllSprites( void* psArrayOfSprites, CGCObject* pcObject );

	// used to get all contained objects so they can be deleted
	//////////////////////////////////////////////////////////////////////////
};

#endif
