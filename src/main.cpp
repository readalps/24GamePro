#include <iostream>
#include "BackwardCalcor.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
    printf(" 24 game pro version 1.0 by Read Alps\n\n");
    printf(" Please input the target non-negative integer to make [default 24]:");
    std::string strInput;
    getline(std::cin, strInput);
    u32 uiVal = 0;
    if (!shiftToU32(strInput, uiVal))
    {
        printf(" Invalid input; use 24 instead.\n");
        uiVal = 24;
    }
    CBackwardCalcor::sm_oAnswer.setValue(uiVal);

    CLog::instance();
    printf(" Do you want to log details in workflow.log? [y/n, default n]:");
    getline(std::cin, strInput);
    if (strInput == "y")
    {
        CLog::instance()->setLogPathAndName(getMainPath().c_str(), "workflow.log");
        CLog::instance()->setLogLevel(LOG_DBG);
    }

    while (true)
    {
        CBackwardCalcor::sm_setResult.clear();
        printf("\n Please input several (at most 6) non-negative integers separated by comma to make %u or q to quit:", uiVal);
        getline(std::cin, strInput);
        trimString(strInput);
        if (strInput == "q")
            break;
        std::vector<std::string> vecStr;
        if (std::string::npos != strInput.find(","))
            divideString2Vec(strInput, ",", vecStr);
        if (isIntStrVec(vecStr) == 0)
        {
            printf(" Invalid input.\n\n");
            continue;
        }
        if (vecStr.size() > 6)
        {
            printf("At most 6 integers.\n");
            continue;
        }
        u32 uiStart = (u32)GetTickCount();
        CBackwardCalcor oCalc(vecStr);
        oCalc.exec();
        if (CBackwardCalcor::sm_setResult.empty())
        {
            printf(" From %s, there is no solution to make %u by using +-*/ and ().\n", strInput.c_str(), uiVal);
            printf(" Used time(ms): %u.\n", GetTickCount() - uiStart);
            continue;
        }
        int nCount = 1;
        for (std::set<std::string>::const_iterator it = CBackwardCalcor::sm_setResult.begin(); it != CBackwardCalcor::sm_setResult.end(); ++it)
            printf(" %d. %s\n", nCount++, it->c_str());
        printf(" Used time(ms): %u.\n", GetTickCount() - uiStart);
    } // end of main loop
    printf("\n");
    return 0;
}
