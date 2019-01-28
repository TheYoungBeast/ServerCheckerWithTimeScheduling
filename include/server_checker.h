#pragma once

#ifndef SERVER_CHECKER_H
#define SERVER_CHECKER_H

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <type_traits>

#include "tcp_client.h"

struct server_data
{
	std::string ip;
	unsigned port;

	template<typename T, typename = std::enable_if_t
	<
		std::conjunction<
			std::disjunction<
				std::is_same< typename T::first_type, std::string >,
				std::is_same< typename T::first_type, const char* >		>,		// !std::disjunction
				std::is_same< typename T::second_type, unsigned >>::value >		// !std::conjunction::value
	>																			// !std::enable_if::type
	server_data(T t_data) : ip(t_data.first), port(t_data.second)
	{};
};

enum class server_status
{
	status_online,
	status_offline,
	status_unknown
};

struct result_data
{
	std::string*	address;
	server_status	status;
};

class server_checker
{
	using servers = std::list<server_data>;

public:
	server_checker(servers* t_list) : m_list(t_list) {};
	~server_checker() {};

	template<typename T, typename = std::enable_if< std::is_function_v<bool(tcp_client*, const std::string&, unsigned)> >::type>
	void check_servers_status(T qualifier)
	{
		for (const auto& v : (*m_list))
		{
			result_data data;

			switch (qualifier(&client, v.ip, v.port))
			{
			case true:
				data.status = server_status::status_online;
				break;
			case false:
				data.status = server_status::status_offline;
				break;
			}

			data.address = const_cast<std::string*> (&v.ip);
			result.push_back(data);
		}
	};

	static bool logical_qualifier(tcp_client* client, const std::string& address, unsigned port)
	{
		client->connect(address, port);

		if (!client->get_status())
			std::cerr << client->error_message() << std::endl;

		return client->get_status();
	};

	auto get_result() const -> const std::list<result_data>& {
		return result;
	}

private:
	tcp_client client;
	std::list<result_data> result;
	const std::list<server_data>* m_list;
};

#endif // !SERVER_CHECKER_H
