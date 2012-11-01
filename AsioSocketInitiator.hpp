/*
AsioQuickfix (c) by Michiel van Slobbe

AsioQuickfix is licensed under a
Creative Commons Attribution-ShareAlike 3.0 Unported License.

You should have received a copy of the license along with this
work.  If not, see <http://creativecommons.org/licenses/by-sa/3.0/>.
*/


#ifndef __SOCKET_INITIATOR_HPP
#define __SOCKET_INITIATOR_HPP

#include "quickfix/Initiator.h"
#include "quickfix/SocketConnector.h"
#include "quickfix/SessionFactory.h"
#include "quickfix/Log.h"
#include "quickfix/SessionID.h"
#include "quickfix/Session.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/resolver_service.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/make_shared.hpp>
#include <boost/signals2.hpp>

#include "AsioSocketConnection.hpp"

namespace FIX
{
	/// Boost asio Socket implementation of Initiator.
	class AsioSocketInitiator :
		public SocketConnector::Strategy, 
		private boost::noncopyable
	{
	public:
		AsioSocketInitiator( boost::asio::io_service & service, 
			Application& application,
			MessageStoreFactory& factory,
			const SessionSettings& settings );
		~AsioSocketInitiator();
		void start();
		void stop();
	private:
		static const char * m_connection_type_label;
		static const char * m_initiator_label;

		boost::asio::io_service & m_service;
		Application& m_application;
		MessageStoreFactory& m_messageStoreFactory;
		SessionSettings m_settings;
		LogFactory* m_pLogFactory;
		Log* m_pLog;

		typedef std::set < SessionID > SessionIDs;
		typedef std::map < SessionID, int > SessionState;
		typedef std::map < SessionID, Session* > Sessions;
		typedef std::map < SessionID, int > SessionToHostNum;
		
		Sessions m_sessions;
		SessionToHostNum m_sessionToHostNum;

		void initialize();
		void getHost( const SessionID&, const Dictionary&, std::string&, short& );
		void ConnectSession( const SessionID& s, const Dictionary& d );

		// Strategy
		virtual void onConnect( SocketConnector&, int socket );
		virtual void onWrite( SocketConnector&, int socket );
		virtual bool onData( SocketConnector&, int socket );
		virtual void onDisconnect( SocketConnector&, int socket );
		virtual void onError( SocketConnector& );
		virtual void onTimeout( SocketConnector& );

		boost::unordered_map < std::string, AsioSocketConnection_ptr > m_connections;
	};

}

#endif
