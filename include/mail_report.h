#pragma once

#ifndef MAIL_REPORT_H
#define MAIL_REPORT_H

#include <list>
#include <string>
#include <vector>
#include <functional>
#include <curl/curl.h>
#include <unordered_map>

#include "server_checker.h"

using StrVec = std::vector<std::string>;

struct EmailData
{
	const StrVec& message;
	size_t index;
};

class mail_report
{
	using m_map = std::unordered_map<std::string, std::reference_wrapper<const std::string>>;

	public:
		mail_report() = default;
		~mail_report() = default;

		void set_email_header(const StrVec&);
		void set_email_message(const std::string&);

		bool parse_header_template();
		bool parse_header_template(const m_map&);

		bool merge();
		bool encode_html_message();
		bool generate_html_message(const std::list<result_data>&, const std::string&);

		bool send_report() const;
		static size_t read_callback(void*, size_t, size_t, void*);

	private:
		mutable CURLcode m_code;
		StrVec m_message;
		std::string m_email_body;

		const std::string smtp_server_url		= "smtp.wp.pl";
		const std::string system_mail_address	= "<notification_system@wp.pl>";
		const std::string system_mail_data		= "GitProject S.A.";
		const std::string system_password		= "notification1133";
		const std::string system_username		= "notification_system@wp.pl";
		const std::string user_mail_data		= "John CC Smith";
		const std::string user_mail_address		= "<serversownercorp@outlook.com>";
		const std::string user_cc_address		= "<serversownercorp@outlook.com>";

		const std::unordered_map<std::string, std::reference_wrapper<const std::string>> EmailParseKeys =
		{
			std::pair< const std::string, std::reference_wrapper<const std::string> >("%TO%"			, user_mail_data),
			std::pair< const std::string, std::reference_wrapper<const std::string> >("%TO_ADDRESS%"	, user_mail_address),
			std::pair< const std::string, std::reference_wrapper<const std::string> >("%FROM%"			, system_mail_data),
			std::pair< const std::string, std::reference_wrapper<const std::string> >("%FROM_ADDRESS%"	, system_mail_address),
			std::pair< const std::string, std::reference_wrapper<const std::string> >("%CC%"			, user_mail_data),
			std::pair< const std::string, std::reference_wrapper<const std::string> >("%CC_ADDRESS%"	, user_cc_address)
		};
};

#endif // !MAIL_REPORT_H