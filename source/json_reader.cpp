#pragma once

#include "json_reader.h"

using namespace rapidjson;

bool read_schedule(const std::string& json, std::vector<std::string>& schedule)
{
	Document document;
	document.Parse( json.c_str() );

	if (!document.IsObject())
		return false;

	if (!document.HasMember("Schedule"))
		return false;

	const auto& ScheduleArray = document[ "Schedule" ];

	if (!ScheduleArray.IsArray())
		return false;

	for (const auto &i : ScheduleArray.GetArray())
		schedule.push_back( i.GetString() );

	return true;
}

bool read_servers_list(const std::string& json, std::list<server_data>& servers_list)
{
	Document document;
	document.Parse( json.c_str() );

	if (!document.IsObject())
		return false;

	if (!document.HasMember("List"))
		return false;

	const auto& ServerArray = document[ "List" ];

	if (!ServerArray.IsArray())
		return false;

	for (const auto &i : ServerArray.GetArray())
	{
		if (!i.IsObject())
			return false;

		auto ip = i.FindMember("ip");
		auto port = i.FindMember("port");

		if (ip != i.MemberEnd() && port != i.MemberEnd())
			servers_list.push_back( std::make_pair( ip->value.GetString(), port->value.GetUint() ) );
		else
			return false;
	}

	return true;
}