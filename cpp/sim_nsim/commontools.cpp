/*
 * commontools.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: elbert
 */

#include <cstring>
#include <string>
#include <ctime>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include "commontools.hpp"
/**
 * String trim
 */
std::string trim(const std::string& str)
{
	unsigned int startpos = 0;
	unsigned int endpos = str.length() - 1;

	if (str.length() == 0)
	{
		return "";
	}

	for (startpos = 0; startpos < str.length(); startpos++)
	{
		if (str[startpos] != ' ' && str[startpos] != '\t' && str[startpos] != '\r' && str[startpos] != '\n')
		{
			break;
		}
	}

	for (endpos = str.length() - 1; endpos >= 0; endpos--)
	{
		if (str[endpos] != ' ' && str[endpos] != '\t' && str[endpos] != '\r' && str[endpos] != '\n')
		{
			break;
		}
	}

	if (endpos >= startpos)
	{
		return str.substr(startpos, endpos - startpos + 1);
	}
	else
	{
		return "";
	}
}

std::string getLocalTimeAndPid(unsigned long curPid)
{
	char localTime[64];
	std::ostringstream rtn;
	memset(localTime, 0, sizeof(localTime));
	// システム時間を取得
	time_t now_time = time(NULL);
	strftime(localTime, sizeof(localTime) - 1, "%Y/%m/%d %X", localtime(&now_time));
	rtn << localTime << ":(" << curPid << ")";
	return rtn.str();
}

/**
 * convert a string to Upper case
 */
std::string & cnvToUpper(std::string & str)
{
	transform(str.begin(),str.end(),str.begin(),toupper);
	return str;
}

bool findCRLF(const std::string & str)
{
	bool rtn = false;
	for (unsigned int i = 0; i < str.length() - 1; i++)
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
		{
			rtn = true;
			break;
		}
	}
	return rtn;

}


