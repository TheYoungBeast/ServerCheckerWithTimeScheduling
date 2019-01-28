#include "mail_report.h"
#include "base64.h"

void mail_report::set_email_header(const StrVec& t_header)	{
	m_message = t_header;									}

void mail_report::set_email_message(const std::string& t_message)	{
	m_email_body = t_message;										}

bool mail_report::parse_header_template()						{
	return mail_report::parse_header_template(EmailParseKeys);	}

bool mail_report::parse_header_template(const m_map& t_ParseKeys)
{
	if (m_message.empty())
		return false;
	
	for (auto& index : m_message)
	{
		for (const auto& key : t_ParseKeys)
		{
			size_t pos = index.find( key.first );

			if (pos == std::string::npos)
				continue;
			else
				index.replace(index.begin() + pos, index.begin() + (pos + key.first.length()), key.second);
		}
	}

	return true;
}

bool mail_report::merge()
{
	if(m_message.empty() || m_email_body.empty())
		return false;

	m_message.push_back(m_email_body);
	return true;
}

bool mail_report::encode_html_message()
{
	if (m_email_body.empty())
		return false;

	m_email_body = base64_encode(reinterpret_cast<unsigned const char*> (m_email_body.c_str()), static_cast<int> (m_email_body.length()) );
	return true;
}

bool mail_report::generate_html_message(const std::list<result_data>& t_Result, const std::string& t_Key)
{
	if (t_Result.empty() || t_Key.empty())
		return false;

	size_t KeyPos = m_email_body.find(t_Key);

	if (KeyPos == std::string::npos)
		return false;

	const std::string TableTr("<tr><td class=\"ip\"><%ip%></td><td class=\"status status<%class-num%>\"><%status%></td></tr>");
	std::vector<std::string> TableVec;

	for (const auto& v : t_Result)
	{
		std::string t_tr(TableTr);

		size_t IpPos = t_tr.find("<%ip%>");
		
		if (IpPos == std::string::npos)
			return false;

		t_tr.replace(t_tr.begin() + IpPos, t_tr.begin() + (IpPos + 6), *(v.address));

		size_t StatusPos = t_tr.find("<%status%>");

		if (StatusPos == std::string::npos)
			return false;

		bool t_status = false;

		switch (v.status)
		{
			case server_status::status_online:
			{
				t_tr.replace(t_tr.begin() + StatusPos, t_tr.begin() + (StatusPos + 10), "ONLINE");
				t_status = true;
				break;
			}
			default:
			{
				t_tr.replace(t_tr.begin() + StatusPos, t_tr.begin() + (StatusPos + 10), "OFFLINE");
				t_status = false;
				break;
			}
		}

		size_t ClassPos = t_tr.find("<%class-num%>");

		if (ClassPos == std::string::npos)
			return false;

		t_tr.replace(t_tr.begin() + ClassPos, t_tr.begin() + (ClassPos + 13), std::to_string(t_status));

		TableVec.push_back( t_tr );
	}

	std::string strTable;

	for (const auto& v : TableVec)
		strTable.append(v);

	m_email_body.replace(m_email_body.begin() + KeyPos, m_email_body.begin() + (KeyPos + t_Key.length()), strTable);
	return true;
}


size_t mail_report::read_callback(void* buffer, size_t size, size_t nitems, void* userdata)
{
	EmailData* msg = reinterpret_cast<EmailData*> (userdata);

	if (!msg)
		return 0;

	if ((size*nitems) < 1)
		return 0;

	if (msg->index < msg->message.size())
	{
		size_t iLen = msg->message[msg->index].length();
		memcpy(buffer, msg->message[msg->index].c_str(), iLen);
		msg->index++;

		return iLen;
	}

	return 0;
}

bool mail_report::send_report() const
{
	m_code = CURLE_OK;
	CURL* curl = nullptr;
	curl_slist* Addressee = nullptr;
	EmailData data{ m_message, 0 };


	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_USERNAME, system_username.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, system_password.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, smtp_server_url.c_str());

		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
		curl_easy_setopt(curl, CURLOPT_CAINFO, "../curl-ca-bundle.crt");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0"); // curl.haxx.se - "some servers don't like requests that are made without a user-agent field, so we provide one"


		curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

		Addressee = curl_slist_append(Addressee, user_mail_address.c_str());
		Addressee = curl_slist_append(Addressee, user_cc_address.c_str());

		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, Addressee);
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, system_mail_address.c_str());

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, mail_report::read_callback); // read_callback might be injected through param. See conception => [ parse_header_template overloads ( mail_report.h ) ]
		curl_easy_setopt(curl, CURLOPT_READDATA, &data);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		m_code = curl_easy_perform(curl);

		curl_slist_free_all(Addressee);
		curl_easy_cleanup(curl);
	}

	return m_code == CURLE_OK;
}