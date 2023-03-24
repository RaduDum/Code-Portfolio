//////////////////////////////////////////////////////////////////////////
// Class responsible for managing the event system

#ifndef _CGCEVENTREGISTRY_H_
#define _CGCEVENTREGISTRY_H_

#include <map>
#include <string>
#include <vector>

//comparator function
struct cmp_str
{
	bool operator()( const char* a, const char* b ) const
	{
		return std::strcmp( a, b ) < 0;
	}
};

class CGCEventListener;

class CGCEventRegistry
{
public:
	CGCEventRegistry( void ) {}

	//maximum amount of listeners able to allocate to 1 event
	const unsigned int k_uMaxListenersPerEvent = 50;

	~CGCEventRegistry( void );

	//registers an event
	void RegisterEvent( const char* pszEventName );

	//registers to listen to an event
	void ListenToEvent( const char*	pszEventName, CGCEventListener* pcListener );

	//sends an event
	void SendEvent( const char* pszEventName );

private:
	//map of event names to list of registered listeners
	//size of vector is pre-set to avoid dynamic resizing
	std::map < const char*, std::vector<CGCEventListener*>* , cmp_str> m_cMsgToListenerMap;
};

#endif