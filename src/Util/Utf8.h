/*!
  @file
  @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_UTIL_UTF8_H_INCLUDED
#define CNOID_UTIL_UTF8_H_INCLUDED

#include <string>
#include "exportdecl.h"

namespace cnoid {
#ifdef _WIN32
    CNOID_EXPORT const std::string toUtf8(const std::string& text);
    CNOID_EXPORT const std::string fromUtf8(const std::string& text);
    CNOID_EXPORT const std::string toUtf8(const char* text);
    CNOID_EXPORT const std::string fromUtf8(const char* text);
#else
    inline const std::string& toUtf8(const std::string& text) { return text; }
    inline const std::string& fromUtf8(const std::string& text) { return text; }
    inline const std::string toUtf8(const char* text) { return text; }
    inline const std::string fromUtf8(const char* text) { return text; }
    //inline std::string toUtf8(const std::string& text) { return text; }
    //inline std::string fromUtf8(const std::string& text) { return text; }
#endif
}

#endif
