#include "GCEventRegistry.h"
#include "GCEventListener.h"

#include "cocos2d.h"
#include "GamerCamp/Core/GCTypes.h"

USING_NS_CC;


CGCEventRegistry::~CGCEventRegistry()
{
	for( std::map < const char*, std::vector<CGCEventListener*>* >::iterator mapIt = m_cMsgToListenerMap.begin();
		 mapIt != m_cMsgToListenerMap.end();
		 ++mapIt )
	{
		//delete the vector holding the pool of event listeners
		//don't need to delete the listeners themselves - not our responsibility, as they are standard game objects
		delete mapIt->second;
		mapIt->second = nullptr;
	}
	m_cMsgToListenerMap.clear();

}

//////////////////////////////////////////////////////////////////////////
void CGCEventRegistry::RegisterEvent( const char* pszEventName )
{
	//event should be non registered yet
	GCASSERT( nullptr == m_cMsgToListenerMap[ pszEventName ], "" );

	std::vector<CGCEventListener*>* pListener = m_cMsgToListenerMap[ pszEventName ];

	if( 0 == m_cMsgToListenerMap[ pszEventName ] )
	{
		m_cMsgToListenerMap[ pszEventName ] = new std::vector<CGCEventListener*>();

		//resize vector to the maximum size of elements it can contain
		m_cMsgToListenerMap[ pszEventName ]->reserve( k_uMaxListenersPerEvent );
	}
}

//////////////////////////////////////////////////////////////////////////
void CGCEventRegistry::ListenToEvent( const char*	pszEventName, CGCEventListener* pcListener )
{
	//listener should be valid
	GCASSERT( nullptr != pcListener, "" );

	//event should exist at this point
	GCASSERT( nullptr != m_cMsgToListenerMap[ strEventName ], "" );

	//listener pool should not be full at this point
	GCASSERT( m_cMsgToListenerMap[ pszEventName ]->size() < k_uMaxListenersPerEvent, "" );

	m_cMsgToListenerMap[ pszEventName ]->push_back( pcListener );
}

//////////////////////////////////////////////////////////////////////////
void CGCEventRegistry::SendEvent( const char* pszEventName )
{
	//event should be valid
	GCASSERT( nullptr != m_cMsgToListenerMap[ pszEventName ], "" );

	std::vector<CGCEventListener*>* pcListeners = m_cMsgToListenerMap[ pszEventName ];

	//notify all subscribers of the message
	for( unsigned int uIndex = 0; uIndex < pcListeners->size(); ++uIndex )
	{
		( *pcListeners )[ uIndex ]->VOnEvent( pszEventName );
	}
}