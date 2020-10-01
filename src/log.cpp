#include "common.h"
#include "log.h"
#include <stdarg.h>
#include <ctime>
#include <sys/timeb.h>
using namespace std;

CLog* CLog::sm_poInstance = NULL;

void CLog::setLogPathAndName(const char* pPath, const char* pszName)
{
    m_strAbsFile = std::string(pPath) + pszName;
    m_pFile = fopen(m_strAbsFile.c_str(), "a+");
}

void CLog::LogMsg(u8 ucLevel, char* pszFmt, ...)
{
    if (!CLog::instance()->meetLogCond(ucLevel))
        return;

    std::string strMsg;
    size_t uiLen = 0;
    {
        va_list va;
        va_start(va, pszFmt);
#ifdef _WINDOWS
        uiLen = _vsnprintf(NULL, 0, pszFmt, va);
#else
        uiLen = vsnprintf(NULL, 0, pszFmt, va);
#endif
        va_end(va);
    }

    if (uiLen > 0)
    {
        strMsg.resize(uiLen);

        va_list va;
        va_start(va, pszFmt);
#ifdef _WINDOWS
        _vsnprintf(((char*)strMsg.c_str()), uiLen + 1, pszFmt, va);
#else
        vsnprintf(((char*)strMsg.c_str()), uiLen + 1, pszFmt, va);
#endif
        va_end(va);
    }
    sm_poInstance->LogMsgInner(ucLevel, strMsg.c_str());
}

void CLog::LogMsgS(u8 ucLevel, const char* pszMsg)
{
    if (CLog::instance()->meetLogCond(ucLevel))
        sm_poInstance->LogMsgInner(ucLevel, pszMsg);
}

void getCurrentTimeString(char* pszOut)
{
    tm* pTm;
    timeb oTimeb;
    ftime(&oTimeb);
    pTm = localtime(&oTimeb.time);
    sprintf(pszOut, "%04d-%02d-%02d %02d:%02d:%02d.%03d",  
        pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec, oTimeb.millitm);  
}

void CLog::LogMsgInner(u8 ucLevel, const char* pszMsg)
{
    char szDateTime[64];
    getCurrentTimeString(szDateTime);
    char szLogLevel[6] = " [2] ";
    if (ucLevel != 2)
        szLogLevel[2] = '0' + ucLevel;
    strcat(szDateTime, szLogLevel);
    fprintf(m_pFile, szDateTime);
    fprintf(m_pFile, pszMsg);
    fprintf(m_pFile, "\n");
    fflush(m_pFile);
}
