/*
 * String Utils
 * A collection of string manipulation functions
 *
 */
#pragma once
#ifndef __STRING_UTILS
#define __STRING_UTILS

#include <string>
#include <vector>

namespace Timewhale {

    /* split
     * This method splits a string into a vector of strings at the given
     * set of delimiters.
     *\param    string      The string to split.
     *\param    string      The set of delimiters to split the string with.
     *\return   vector      A vector of strings.        */
    std::vector<std::string> split (const std::string &line, const std::string &delimiters);

    std::wstring toWideString(const std::string &str);
}

#define STR_HELP(x) #x
#define STR(arg) STR_HELP(arg)

#define BoolToStr(expr) \
	(expr) ? "True" : "False" 
#define BoolToChar(expr) \
	(expr) ? 'T' : 'F'

#endif
