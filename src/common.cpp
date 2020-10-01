#include "common.h"
#ifdef _WINDOWS
#include <direct.h>
#include <windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
u32 GetTickCount()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (u32)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
#endif

void trimString(std::string& strTrim)
{
    size_t iHeadPos = strTrim.find_first_not_of(" \t");
    if (iHeadPos == std::string::npos)
    {
        strTrim = "";
        return;
    }
    size_t iTailPos = strTrim.find_last_not_of(" \t");
    if (strTrim.length() != iTailPos + 1)
        strTrim.erase(iTailPos + 1);
    if (0 != iHeadPos)
        strTrim.erase(0, iHeadPos);
}

bool separateString(const std::string& strSrc, const char* pszSep, std::string& strHead, std::string& strTail, bool bFromLeft)
{
    std::string::size_type iPos = (bFromLeft ? strSrc.find(pszSep) : strSrc.rfind(pszSep));
    if (iPos == std::string::npos)
    {
        strHead = strSrc;
        trimString(strHead);
        strTail = "";
        return false;
    }

    if (0 == iPos)
        strHead = "";
    else
    {
        strHead = strSrc.substr(0, iPos);
        trimString(strHead);
    }
    if (strSrc.length() == iPos + strlen(pszSep))
        strTail = "";
    else
    {
        strTail = strSrc.substr(iPos + strlen(pszSep));
        trimString(strTail);
    }
    return true;
}

void divideString2Vec(const std::string& strInput, const char* pszSep, std::vector<std::string>& vecOut)
{
    std::string strHead, strTail;
    std::string strRights = strInput;
    while (!strRights.empty())
    {
        separateString(strRights, pszSep, strHead, strTail, true);
        trimString(strHead);
        if (!strHead.empty())
            vecOut.push_back(strHead);
        trimString(strTail);
        strRights = strTail;
    }
}

bool isIntStr(const std::string& strVal)
{
    if (strVal.empty())
        return false;
    size_t len = strVal.length();
    if (len >= 10)
        return false;
    for (size_t idx = 0; idx < len; ++idx)
    {
        if (strVal[idx] < '0' || strVal[idx] > '9')
            return false;
    }
    return true;
}

bool shiftToU32(std::string& strVal, u32& uiVal)
{
    trimString(strVal);
    if (strVal.empty())
    {
        uiVal = 24;
        return true;
    }
    if (!isIntStr(strVal))
        return false;
    uiVal = strtoul(strVal.c_str(), 0, 10);
    return true;
}

std::string u32ToStr(u32 uiVal)
{
    char szBuf[128];
    sprintf(szBuf, "%u", uiVal);
    return szBuf;
}

bool isIntStrVec(std::vector<std::string>& vecVal)
{
    if (vecVal.empty())
        return false;
    for (size_t idx = 0; idx < vecVal.size(); ++idx)
    {
        if (!isIntStr(vecVal[idx]))
            return false;
    }
    return true;
}

static std::string s_strMainPath;
std::string getMainPath()
{
    if (!s_strMainPath.empty())
        return s_strMainPath;

    char szBuff[512];
#ifdef _WINDOWS
    GetModuleFileName(NULL, szBuff, 512);
    std::string strAbsFile = szBuff;
    size_t iPos = strAbsFile.rfind(DIR_SLASH);
    s_strMainPath = strAbsFile.substr(0, iPos + 1);
#else
    getcwd(szBuff, 512);
    s_strMainPath = std::string(szBuff) + "/";
#endif
    return s_strMainPath;
}
