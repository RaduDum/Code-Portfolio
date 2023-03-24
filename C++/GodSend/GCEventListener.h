//interface for an event listener to implement

#ifndef _GCEVENTLISTENER_H_
#define _GCEVENTLISTENER_H_

#include "GCEventRegistry.h"

//////////////////////////////////////////////////////////////////////////
class CGCEventListener
{
public:
	//pure virtual function for passing an event
	virtual void VOnEvent( const char* pszEventName ) = 0;

	virtual ~CGCEventListener() {};
};

#endif