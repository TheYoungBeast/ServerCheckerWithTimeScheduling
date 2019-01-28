#pragma once

#include <map>
#include <vector>

#include "files.h"
#include "base64.h"

const std::string read_file(const char* Path)
{
	std::fstream fFile(Path, std::ios_base::in);

	if (!fFile.is_open())
		return std::string();

	std::string buffer( std::istreambuf_iterator<std::ifstream::char_type>(fFile),
					(	std::istreambuf_iterator<std::ifstream::char_type>()	));
	fFile.close();

	return buffer;
}

StringVec read_line_by_line(const char* Path)
{
	StringVec header;

	std::ifstream file(Path, std::ios_base::in);

	if (!file.is_open())
		return header;

	std::string buffer;
	while (getline(file, buffer))
	{
		buffer += '\n';
		header.push_back(buffer);
		buffer.erase();
	}

	return header;
}