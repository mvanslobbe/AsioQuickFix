#ifndef __ASIO_SOCKET_CONNECTION_HPP
#define __ASIO_SOCKET_CONNECTION_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "quickfix/Session.h"
#include "quickfix/Parser.h"

#define INCOMING_BUFFER_SIZE 8192
#define OUTGOING_BUFFER_SIZE 8192

namespace FIX
{
	class AsioSocketConnection;
	typedef boost::shared_ptr < AsioSocketConnection > AsioSocketConnection_ptr;

	class AsioSocketConnection : private boost::noncopyable, 
		public Responder
	{
	public:
		AsioSocketConnection ( boost::shared_ptr < boost::asio::ip::tcp::socket > const & socket,
			SessionID const & sessionID,
			Session * session	);
		~AsioSocketConnection();

		 virtual bool send( const std::string& );
		virtual void disconnect();
	private:
		boost::shared_ptr < boost::asio::ip::tcp::socket > m_socket;
		SessionID m_session_id;
		Session * m_session;
		Parser m_parser;
		boost::array<char, INCOMING_BUFFER_SIZE> m_incoming_buffer;
		boost::array<char, OUTGOING_BUFFER_SIZE> m_outgoing_buffer;
		std::vector<char> m_queued_outgoing_buffer;
		bool m_sending;

		void StartSendAsync();
		void StartReadAsync();
		void AsyncReadSocket( boost::system::error_code const & error_code, 
			size_t len );
		void AsyncSentSocket( boost::system::error_code const & error_code, 
			size_t len );
	};

}


#endif
