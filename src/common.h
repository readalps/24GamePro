#ifndef CMN_H
#define CMN_H

#include <stdint.h>
#include <string>
#include <vector>
#ifdef _LINUX
#include <string.h>
#endif
using namespace std;

#ifdef _WINDOWS
#define DIR_SLASH       '\\'
#define DIR_SLASH_STR   "\\"
#else
#define DIR_SLASH       '/'
#define DIR_SLASH_STR   "/"
#endif

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;

void trimString(std::string& strTrim);
bool separateString(const std::string& strSrc, const char* pszSep, std::string& strHead, std::string& strTail, bool bFromLeft);
void divideString2Vec(const std::string& strInput, const char* pszSep, std::vector<std::string>& vecOut);
bool shiftToU32(std::string& strVal, u32& uiVal);
bool isIntStrVec(std::vector<std::string>& vecVal);
std::string u32ToStr(u32 uiVal);
#ifdef _LINUX
u32 GetTickCount();
#endif

std::string getMainPath();

#endif
