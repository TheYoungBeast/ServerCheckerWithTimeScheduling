#pragma once

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class tcp_client
{
	public:
		tcp_client() {};
		~tcp_client() {};
		void connect(const std::string&, const unsigned);
		bool get_status() const;
		auto error_message() const -> const std::string;

	private:
		bool m_status = false;
		std::string m_errormsg;
};

#endif // !TCP_CLIENT_H