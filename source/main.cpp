#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <string>

#include "files.h"
#include "json_reader.h"
#include "mail_report.h"
#include "server_checker.h"

int main()
{
	std::list<server_data> servers;
	std::vector<std::string> schedule;
	std::string json_schedule( read_file("../data/schedule.json") );
	std::string json_servers( read_file("../data/servers-list.json") );

	if (!read_schedule(json_schedule, schedule))
	{
		std::cerr << "There was an error reading the schedule.\n";
		return 0;
	}

	if (!read_servers_list(json_servers, servers))
	{
		std::cerr << "There was an error reading the servers list.\n";
		return 0;
	}

	/************************************************************************************************
		↓↓↓↓↓↓↓↓↓↓↓↓ Could be turned into primitive Facade Pattern to simplify access ↓↓↓↓↓↓↓↓↓↓↓↓
	**************************************************************************************************/
	server_checker Checker(&servers);
	Checker.check_servers_status( server_checker::logical_qualifier );

	mail_report report;
	report.set_email_header( read_line_by_line("../data/email_template.txt") );
	report.set_email_message( read_file("../data/email_pattern.html") );
	report.parse_header_template();
	report.generate_html_message(Checker.get_result(), "<%table%>");
	report.encode_html_message();
	report.merge();
	report.send_report();

	return 0;
}