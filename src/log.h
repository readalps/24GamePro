#ifndef LOG_MGR_H
#define LOG_MGR_H

#include "common.h"
#include <stdio.h>

enum EnumLogLevel
{
    LOG_ERR = 0,
    LOG_WRN = 1,
    LOG_DBG = 2
};

class CLog
{
public:
    void setLogPathAndName(const char* pPath, const char* pszName);
    static void LogMsg(u8 ucLevel, char* pszFmt, ...);
    static void LogMsgS(u8 ucLevel, const char* pszMsg);
    u8 getLogLevel()
    {
        return m_ucLevel;
    }
    void setLogLevel(u8 ucLevel)
    {
        if (ucLevel >= LOG_ERR && ucLevel <= LOG_DBG)
            m_ucLevel = ucLevel;
    }
    bool meetLogCond(u8 ucLevel)
    {
        return (NULL != m_pFile && ucLevel <= m_ucLevel);
    }

    static CLog* instance()
    {
        if (NULL == sm_poInstance)
            sm_poInstance = new CLog();
        return sm_poInstance;
    }

private:
    CLog()
    {
        m_pFile = NULL;
        m_ucLevel = LOG_ERR;
    }
    void LogMsgInner(u8 ucLevel, const char* pszMsg);

private:
    std::string m_strAbsFile;
    u8 m_ucLevel;
    FILE* m_pFile;

    static CLog* sm_poInstance;
};

#endif
