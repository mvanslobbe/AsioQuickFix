#include "AsioSocketInitiator.hpp"

namespace FIX
{
	AsioSocketInitiator::AsioSocketInitiator( boost::asio::io_service & service, 
		Application& application,
		MessageStoreFactory& factory,
		const SessionSettings& settings ) :
	m_service ( service ),
		m_application( application ),
		m_messageStoreFactory( factory ),
		m_settings( settings ),
		m_pLogFactory( 0 ),
		m_pLog( 0 )
	{
		initialize();
	}

	AsioSocketInitiator::~AsioSocketInitiator()
	{
		stop();
	}

	void AsioSocketInitiator::start()
	{
	}

	void AsioSocketInitiator::stop()
	{
		for ( boost::unordered_map < std::string, AsioSocketConnection_ptr >::iterator iter = m_connections.begin();
			iter != m_connections.end();
			iter ++ ) 
			iter->second->disconnect();
		m_connections.clear();	
	}

	// just like the Initiator::initialize function
	void AsioSocketInitiator::initialize()
	{
		std::set < SessionID > sessions = m_settings.getSessions();
		std::set < SessionID > ::iterator i;

		if ( !sessions.size() )
			throw ConfigError( "No sessions defined" );

		SessionFactory factory( m_application, m_messageStoreFactory,
			m_pLogFactory );

		for ( i = sessions.begin(); i != sessions.end(); ++i )
		{
			if ( m_settings.get( *i ).getString( "ConnectionType" ) == "initiator" )
			{
				m_sessions[ *i ] = factory.create( *i, m_settings.get( *i ) );
				ConnectSession ( *i, m_settings.get( *i ) );
			}
		}

		if ( !m_sessions.size() )
			throw ConfigError( "No sessions defined for initiator" );
	}

	void AsioSocketInitiator::ConnectSession( const SessionID& s, const Dictionary& d )
	{
		std::string address;
		short port = 0;
		Session* session = Session::lookupSession( s );
		assert ( session != 0 ) ;
		if( !session->isSessionTime(UtcTimeStamp()) ) return;

		getHost( s, d, address, port );

		boost::shared_ptr < boost::asio::ip::tcp::socket > socket ( new boost::asio::ip::tcp::socket ( m_service ) );
		boost::asio::ip::tcp::resolver::query query ( boost::asio::ip::tcp::v4(),
			address,
			boost::lexical_cast < std::string > ( port ) );
		boost::asio::ip::tcp::resolver resolver ( m_service );
		boost::system::error_code ec;
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve ( query, ec );

		if ( ec != 0 )
			throw new std::runtime_error ( ( boost::format ( "Unable to resolve [%1%][%2%]" ) % address % port ).str().c_str() );
		assert ( socket.get() != 0 );
		// we could have done this async, but it's easier this way ..
		socket->connect ( *iterator, ec );

		if ( ec != 0 ) 
			throw new std::runtime_error ( ( boost::format ( "Unable to connect to [%1%][%2%]" ) % address % port ).str().c_str() );

		AsioSocketConnection_ptr connection ( boost::make_shared < AsioSocketConnection > ( socket,
			s,
			session ) );
		m_connections.insert ( std::make_pair ( s.toString(), connection ) );
	}

	void AsioSocketInitiator::getHost( const SessionID& s, const Dictionary& d,
		std::string& address, short& port )
	{ 
		int num = 0;
		SessionToHostNum::iterator i = m_sessionToHostNum.find( s );
		if ( i != m_sessionToHostNum.end() ) num = i->second;

		std::stringstream hostStream;
		hostStream << SOCKET_CONNECT_HOST << num;
		std::string hostString = hostStream.str();

		std::stringstream portStream;
		std::string portString = portStream.str();
		portStream << SOCKET_CONNECT_PORT << num;

		if( d.has(hostString) && d.has(portString) )
		{
			address = d.getString( hostString );
			port = ( short ) d.getLong( portString );
		}
		else
		{
			num = 0;
			address = d.getString( SOCKET_CONNECT_HOST );
			port = ( short ) d.getLong( SOCKET_CONNECT_PORT );
		}

		m_sessionToHostNum[ s ] = ++num;
	}

	void AsioSocketInitiator::onConnect( SocketConnector&, int socket ) 
	{
	}
	void AsioSocketInitiator::onWrite( SocketConnector&, int socket ) {}
	bool AsioSocketInitiator::onData( SocketConnector&, int socket ) { return true; }
	void AsioSocketInitiator::onDisconnect( SocketConnector&, int socket ) {}
	void AsioSocketInitiator::onError( SocketConnector& ) {}
	void AsioSocketInitiator::onTimeout( SocketConnector& ) {}
}

