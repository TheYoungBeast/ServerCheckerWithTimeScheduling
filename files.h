#pragma once

#ifndef FILES_H
#define FILES_H


#include <string>
#include <vector>
#include <fstream>

typedef std::vector<std::string> StringVec;

const std::string read_file(const char*);
StringVec read_line_by_line(const char*);

#endif // !FILES_H