#pragma once

#ifndef JSON_READER_H
#define JSON_READER_H


#include <list>
#include <vector>
#include <string>
#include <rapidjson/document.h>

#include "server_checker.h"

bool read_schedule(const std::string&, std::vector<std::string>&);
bool read_servers_list(const std::string&, std::list<server_data>&);

#endif // !JSON_READER_H