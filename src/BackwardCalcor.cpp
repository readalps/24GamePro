#include "BackwardCalcor.h"
#ifdef _LINUX
#include <stdlib.h>
#endif

SFraction minusByZero(const SFraction& oVal)
{
    SFraction oRet = oVal;
    oRet.bNegative = (!oRet.bNegative);
    return oRet;
}

SFraction fractAdd(const SFraction& oL, const SFraction& oR)
{
    SFraction oRslt;
    /// having same denominator
    if (oL.denominator == oR.denominator)
    {
        oRslt.denominator = oL.denominator;
        if (oL.bNegative == oR.bNegative)
        {
            oRslt.numerator = (oL.numerator + oR.numerator);
            oRslt.bNegative = oL.bNegative;
            simplifyFraction(oRslt);
            return oRslt;
        }
        bool bCmp = oL.numerator >= oR.numerator;
        oRslt.numerator = (bCmp ? oL.numerator - oR.numerator : oR.numerator - oL.numerator);
        oRslt.bNegative = (bCmp ? oL.bNegative : oR.bNegative);
        simplifyFraction(oRslt);
        return oRslt;
    }
    /// having different denominator
    u64 lcm = calcLeastCommonMultiple(oL.denominator, oR.denominator);
    oRslt.denominator = lcm;
    u64 numL = oL.numerator * (lcm / oL.denominator);
    u64 numR = oR.numerator * (lcm / oR.denominator);
    if (oL.bNegative == oR.bNegative)
    {
        oRslt.numerator = numL + numR;
        oRslt.bNegative = oL.bNegative;
        simplifyFraction(oRslt);
        return oRslt;
    }
    bool bCmp = (numL >= numR);
    oRslt.numerator = (bCmp ? numL - numR : numR - numL);
    oRslt.bNegative = (bCmp ? oL.bNegative : oR.bNegative);
    simplifyFraction(oRslt);
    return oRslt;
}

SFraction reciprocal(const SFraction& oVal)
{
    SFraction oRet;
    oRet.numerator = oVal.denominator;
    oRet.denominator = oVal.numerator;
    oRet.bNegative = oVal.bNegative;
    return oRet;
}

SFraction fractMultiply(const SFraction& oL, const SFraction& oR)
{
    SFraction oRslt;
    oRslt.numerator = oL.numerator * oR.numerator;
    oRslt.denominator = oL.denominator * oR.denominator;
    oRslt.bNegative = (oL.bNegative != oR.bNegative);
    simplifyFraction(oRslt);
    return oRslt;
}

u64 calcGcdInn(u64 valBig, u64 valSmall)
{
    u64 remainder = valBig % valSmall;
    if (remainder == 0)
    {
        return valSmall;
    }
    return calcGcdInn(valSmall, remainder);
}

u64 calcGreatestCommonDivisor(u64 valA, u64 valB)
{
    if (valA == valB)
        return valA;
    return (valA > valB ? calcGcdInn(valA, valB) : calcGcdInn(valB, valA));
}

u64 calcLeastCommonMultiple(u64 valA, u64 valB)
{
    return valA / calcGreatestCommonDivisor(valA, valB) * valB;
}

void simplifyFraction(SFraction& oFract)
{
    if (oFract.denominator == 1) return;
    if (oFract.numerator == 0)
    {
        oFract.denominator = 1;
        return;
    }
    u64 gcd = calcGreatestCommonDivisor(oFract.numerator, oFract.denominator);
    if (gcd != 1)
    {
        oFract.numerator /= gcd;
        oFract.denominator /= gcd;
    }
}

SFraction CBackwardCalcor::sm_oAnswer;
std::set<std::string> CBackwardCalcor::sm_setResult;

CBackwardCalcor::CBackwardCalcor(const std::vector<std::string>& vecStr)
{
    SExpress oExp;
    for (size_t idx = 0; idx < vecStr.size(); idx++)
    {
        oExp.oVal.setValue(strtoul(vecStr[idx].c_str(), 0, 10));
        oExp.strExpress = vecStr[idx];
        m_vecExpress.push_back(oExp);
    }
}

CBackwardCalcor::CBackwardCalcor(const std::vector<SExpress>& vecExp)
{
    m_vecExpress = vecExp;
}

void CBackwardCalcor::setAscendingOrder()
{
    for (size_t idx = 1; idx < m_vecExpress.size(); idx++){
        for (size_t preidx = 0; preidx < idx; preidx++){
            if (m_vecExpress[idx].oVal < m_vecExpress[preidx].oVal){
                SExpress oLow = m_vecExpress[idx];
                for (size_t mvidx = idx; mvidx > preidx; --mvidx)
                    m_vecExpress[mvidx] = m_vecExpress[mvidx - 1];
                m_vecExpress[preidx] = oLow;
            }
        }
    }
}

void CBackwardCalcor::stepOver(size_t& seq1, size_t& seq2)
{
    if (seq2 + 1 < m_vecExpress.size())
    {
        ++seq2;
        return;
    }
    ++seq1;
    seq2 = seq1 + 1;
}

void CBackwardCalcor::workOnLevel(size_t seq1, size_t seq2)
{
    std::vector<SExpress> vecLeft;
    for (size_t idx = 0; idx < m_vecExpress.size(); ++idx)
    {
        if (idx != seq1 && idx != seq2)
            vecLeft.push_back(m_vecExpress[idx]);
    }

    if (CLog::instance()->meetLogCond(LOG_DBG))
    {
        std::string str = "no pending nodes";
        if (!vecLeft.empty())
        {
            str = u32ToStr(vecLeft.size()) + " pending node(s):";
            for (size_t idx = 0; idx < vecLeft.size(); idx++)
                str += (" " + vecLeft[idx].strExpress);
        }
        CLog::LogMsg(LOG_DBG, "cur two: L=%s, R=%s with %s", m_vecExpress[seq1].strExpress.c_str(), m_vecExpress[seq2].strExpress.c_str(), str.c_str());
    }

    reduct(E_OP_ADD, seq1, seq2, vecLeft);
    reduct(E_OP_MULTIPLE, seq1, seq2, vecLeft);
    reduct(E_OP_R_SUBST, seq1, seq2, vecLeft);
    reduct( E_OP_DIVIDE, seq1, seq2, vecLeft);
    reduct(E_OP_R_DIVIDE, seq1, seq2, vecLeft);
}

bool CBackwardCalcor::isRedundant(EnumOp eOpL, EnumOp eOpR, EnumOp eOp)
{
    /// curb a-b-c, a-(b-c), a+(b-c) and a-b+c
    if (eOp == E_OP_R_SUBST || eOp == E_OP_ADD)
    {
        if (eOpL == E_OP_R_SUBST || eOpR == E_OP_R_SUBST)
            return true;
    }
    /// curb a/b/c, a/(b/c), a*(b/c) and a/b*c
    if (eOp == E_OP_MULTIPLE || IS_DIVIDE(eOp))
    {
        if (IS_DIVIDE(eOpL) || IS_DIVIDE(eOpR))
            return true;
    }
    /// curb (a+b)+(c+d)
    if (eOp == E_OP_ADD && eOpL == E_OP_ADD && eOpR == E_OP_ADD)
        return true;
    /// curb (a*b)*(c*d)
    if (eOp == E_OP_MULTIPLE && eOpL == E_OP_MULTIPLE && eOpR == E_OP_MULTIPLE)
        return true;

    return false;
}

bool CBackwardCalcor::isRedundantEx(const SExpress& oExp1, const SExpress& oExp2, EnumOp eOp)
{
    if (eOp == E_OP_MULTIPLE) // consider a*b*c
    {
        if (oExp1.eOp != E_OP_MULTIPLE && oExp2.eOp != E_OP_MULTIPLE)
            return false;
        return (oExp2.eOp != E_OP_MULTIPLE ? true : oExp1.oVal > oExp2.oL);
    }
    if (eOp == E_OP_ADD) // consider a+b+c
    {
        if (oExp1.eOp != E_OP_ADD && oExp2.eOp != E_OP_ADD)
            return false;
        return (oExp2.eOp != E_OP_ADD ? true : oExp1.oVal > oExp2.oL);
    }
    return false;
}

void CBackwardCalcor::reduct(EnumOp eOp, size_t seq1, size_t seq2, const std::vector<SExpress>& vecLeft)
{
    SExpress& oExp1 = m_vecExpress[seq1];
    SExpress& oExp2 = m_vecExpress[seq2];
    if (eOp == E_OP_R_SUBST && oExp1.oVal.isZero()) // curb b-0, only use 0+b
        return;
    if ((eOp == E_OP_DIVIDE || eOp == E_OP_R_DIVIDE) && oExp1.oVal.isZero())
        return;
    if (eOp == E_OP_DIVIDE && oExp1.oVal.isOne()) // curb b/1, only use 1*b
        return;
    if (eOp == E_OP_R_DIVIDE && oExp1.oVal == oExp2.oVal)
        return;
    if (isRedundant(oExp1.eOp, oExp2.eOp, eOp))
    {
        CLog::LogMsg(LOG_DBG, "base curbed: L=%s, R=%s, Op=%d.", oExp1.strExpress.c_str(), oExp2.strExpress.c_str(), (int)eOp);
        return;
    }
    if (isRedundantEx(oExp1, oExp2, eOp))
    {
        CLog::LogMsg(LOG_DBG, "ex curbed: L=%s, R=%s, Op=%d.", oExp1.strExpress.c_str(), oExp2.strExpress.c_str(), (int)eOp);
        return;
    }

    SExpress oReduce;
    doBiOp(eOp, oExp1, oExp2, oReduce, vecLeft.empty());
    if (vecLeft.empty())
    {
        if (CBackwardCalcor::sm_oAnswer == oReduce.oVal)
        {
            if (CLog::instance()->meetLogCond(LOG_DBG))
            {
                CLog::LogMsg(LOG_DBG, "solution: L=%s,R=%s,op=%d => %s", oExp1.strExpress.c_str(), oExp2.strExpress.c_str(), (int)eOp, oReduce.strExpress.c_str());
                if (CBackwardCalcor::sm_setResult.find(oReduce.strExpress) != CBackwardCalcor::sm_setResult.end())
                {
                    CLog::LogMsg(LOG_DBG, "solution: met a repeated solution %s", oReduce.strExpress.c_str());
                }
            }
            CBackwardCalcor::sm_setResult.insert(oReduce.strExpress);
        }
        return;
    }
    std::vector<SExpress> vecNew = vecLeft;
    vecNew.push_back(oReduce);
    CBackwardCalcor oNew(vecNew);
    oNew.exec();
}

std::string putTogether(const SExpress& oExpL, const SExpress& oExpR, EnumOp eOp, 
                        const std::string& strHead, const std::string& strTail)
{
    std::string strL = oExpL.strExpress, strR = oExpR.strExpress;
    if (eOp == E_OP_ADD)
    {
        if (oExpL.strExpress[0] == '(')
            strL = oExpL.strExpress.substr(1, oExpL.strExpress.length() - 2);
        if (oExpR.strExpress[0] == '(')
            strR = oExpR.strExpress.substr(1, oExpR.strExpress.length() - 2);
    }
    else if (eOp == E_OP_R_SUBST)
    {
        if (oExpL.strExpress[0] == '(')
            strL = oExpL.strExpress.substr(1, oExpL.strExpress.length() - 2);
        if (IS_MULTI_OR_DIVE(oExpR.eOp))
            strR = oExpR.strExpress.substr(1, oExpR.strExpress.length() - 2);
    }
    else if (eOp == E_OP_MULTIPLE)
    {
        if (IS_MULTI_OR_DIVE(oExpL.eOp))
            strL = oExpL.strExpress.substr(1, oExpL.strExpress.length() - 2);
        if (IS_MULTI_OR_DIVE(oExpR.eOp))
            strR = oExpR.strExpress.substr(1, oExpR.strExpress.length() - 2);
    }
    else // divide
    {
        if (IS_MULTI_OR_DIVE(oExpL.eOp))
            strL = oExpL.strExpress.substr(1, oExpL.strExpress.length() - 2);
    }

    return strHead + strL + getOpString(eOp) + strR + strTail;
}

void CBackwardCalcor::doBiOp(EnumOp eOp, const SExpress& oExp1, const SExpress& oExp2, SExpress& oReduct, bool bNoLeft)
{
    oReduct.eOp = eOp;
    oReduct.oL = oExp1.oVal;
    oReduct.oR = oExp2.oVal;
    std::string strHead = (bNoLeft ? "" : "(");
    std::string strTail = (bNoLeft ? "" : ")");
    switch (eOp)
    {
    case E_OP_ADD:
    case E_OP_MULTIPLE:
        oReduct.oVal = (eOp == E_OP_ADD ? fractAdd(oExp1.oVal, oExp2.oVal) : fractMultiply(oExp1.oVal, oExp2.oVal));
        if (oExp1.oVal == oExp2.oVal && oExp1.strExpress > oExp2.strExpress)
            oReduct.strExpress = putTogether(oExp2, oExp1, eOp, strHead, strTail);
        else
            oReduct.strExpress = putTogether(oExp1, oExp2, eOp, strHead, strTail);
        break;
    case E_OP_R_SUBST:
    case E_OP_DIVIDE:
        oReduct.oVal = (eOp == E_OP_R_SUBST ? fractAdd(oExp2.oVal, minusByZero(oExp1.oVal)) : 
            fractMultiply(oExp2.oVal, reciprocal(oExp1.oVal)));
        if (oExp1.oVal == oExp2.oVal && oExp1.strExpress > oExp2.strExpress)
            oReduct.strExpress = putTogether(oExp1, oExp2, eOp, strHead, strTail);
        else
            oReduct.strExpress = putTogether(oExp2, oExp1, eOp, strHead, strTail);
        break;
    case E_OP_R_DIVIDE:
    default:
        oReduct.oVal = fractMultiply(oExp1.oVal, reciprocal(oExp2.oVal));
        oReduct.strExpress = putTogether(oExp1, oExp2, eOp, strHead, strTail);
        break;
    }
}

bool CBackwardCalcor::higherLevel(size_t seq1, size_t seq2)
{
    return (m_vecExpress[seq1].oVal > m_oCurValL ? true : m_vecExpress[seq2].oVal > m_oCurValR);
}

bool CBackwardCalcor::topLevel(size_t seq1, size_t seq2)
{
    size_t total = m_vecExpress.size();
    return (m_vecExpress[seq1].oVal == m_vecExpress[total - 2].oVal && m_vecExpress[seq2].oVal == m_vecExpress[total - 1].oVal);
}

void CBackwardCalcor::exec()
{
    setAscendingOrder();

    if (CLog::instance()->meetLogCond(LOG_DBG))
    {
        std::string str = "Dealing:";
        for (size_t idx = 0; idx < m_vecExpress.size(); idx++)
            str += (" " + m_vecExpress[idx].strExpress);
        CLog::LogMsgS(LOG_DBG, str.c_str());
    }

    if (m_vecExpress.size() == 1)
    {
        if (m_vecExpress[0].oVal == sm_oAnswer)
            sm_setResult.insert(m_vecExpress[0].strExpress);
        return;
    }
    if (sm_oAnswer.isZero() && m_vecExpress[m_vecExpress.size() - 1].oVal.isZero())
    {
        std::string str = "0";
        for (size_t idx = 1; idx < m_vecExpress.size(); idx++)
            str += "+0";
        sm_setResult.insert(str);
        return;
    }

    size_t seq1 = 0, seq2 = 1;
    while (seq1 + 1 < m_vecExpress.size())
    {
        if (higherLevel(seq1, seq2))
        {
            m_oCurValL = m_vecExpress[seq1].oVal;
            m_oCurValR = m_vecExpress[seq2].oVal;
            workOnLevel(seq1, seq2);
            if (topLevel(seq1, seq2))
                break;
        }
        stepOver(seq1, seq2);
    }
}
