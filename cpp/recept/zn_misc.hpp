#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

bool mkpath(const std::string &file_path);
bool remove_with_directory(const std::string &filename);

std::string get_current_datetime();

void get_word(const std::string &line, std::size_t &pos, std::string &word);

void replace_charactor_all(std::string& str, const char* search_char, const char* rep_char);

std::string dirname(const std::string &filename);
std::string basename(const std::string &filename);

std::string readfile(const std::string &filename);
bool writefile(const std::string &filename, const std::string &data, std::ios_base::openmode mode = std::ios_base::out);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
std::string join(std::vector<std::string> &v, char delim);

template<typename T>
std::string to_string(T i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

template<typename T>
T from_string(const std::string &str)
{
	std::istringstream iss(str);
	T value;
	iss >> value;
	return value;
}
