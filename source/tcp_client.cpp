#include "tcp_client.h"


void tcp_client::connect(const std::string& address, const unsigned port)
{
	try
	{
		boost::asio::io_service io_service;
		tcp::socket socket(io_service);

		tcp::endpoint end_point(boost::asio::ip::address::from_string(address.c_str()), port);
		socket.connect(end_point);
		m_status = true;
	}

	catch (boost::system::system_error e)
	{
		m_errormsg.erase();
		m_errormsg = e.what();
		m_status = false;
	}

	/*socket.close();*/ // destructor will call close itself
}

bool tcp_client::get_status() const	{
	return m_status;				}

const std::string tcp_client::error_message() const	{
	return m_errormsg;								}
