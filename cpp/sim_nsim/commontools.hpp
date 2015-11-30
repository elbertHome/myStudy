/*
 * commontools.hpp
 *
 *  Created on: Jan 2, 2015
 *      Author: elbert
 */

#ifndef COMMONTOOLS_HPP_
#define COMMONTOOLS_HPP_

#include <unistd.h>

/**
 * trim the sttring
 */
std::string trim(const std::string& str);

/**
 * get local time
 */
std::string getLocalTimeAndPid(unsigned long pid=getpid());

/**
 * convert a string to Upper case
 */
std::string & cnvToUpper(std::string & str);

/**
 * if contains CRLF return true, else return false
 */
bool findCRLF(const std::string & str);


#endif /* COMMONTOOLS_HPP_ */
