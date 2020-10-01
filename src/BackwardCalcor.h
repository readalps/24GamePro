#pragma once

#include "log.h"
#include <set>

struct SFraction;

SFraction minusByZero(const SFraction& oVal);
SFraction fractAdd(const SFraction& oL, const SFraction& oR);

struct SFraction
{
    u64 numerator;
    u64 denominator;
    bool bNegative;

    SFraction()
    {
        reset();
    }
    void reset()
    {
        numerator = 0;
        denominator = 1;
        bNegative = false;
    }
    void setValue(u32 uiVal)
    {
        numerator = (u64)uiVal;
        denominator = 1;
        bNegative = false;
    }
    bool isZero() const
    {
        return (numerator == 0);
    }
    bool isOne() const
    {
        return (numerator != 0 && !bNegative && numerator == denominator);
    }
    bool operator==(const SFraction& oVal) const
    {
        SFraction oResult = fractAdd(*this, minusByZero(oVal));
        return (oResult.numerator == 0);
    }
    bool operator<(const SFraction& oVal) const
    {
        SFraction oResult = fractAdd(*this, minusByZero(oVal));
        return (oResult.numerator != 0 && oResult.bNegative);
    }
    bool operator>(const SFraction& oVal) const
    {
        SFraction oResult = fractAdd(*this, minusByZero(oVal));
        return (oResult.numerator != 0 && !oResult.bNegative);
    }
};

u64 calcGreatestCommonDivisor(u64 valA, u64 valB);
u64 calcLeastCommonMultiple(u64 valA, u64 valB);
void simplifyFraction(SFraction& oFract);

enum EnumOp{E_OP_NONE, E_OP_ADD, E_OP_R_SUBST, E_OP_MULTIPLE, E_OP_DIVIDE, E_OP_R_DIVIDE};

#define IS_DIVIDE(op) (op == E_OP_DIVIDE || op == E_OP_R_DIVIDE)
#define IS_MULTI_OR_DIVE(op) (op > E_OP_R_SUBST)
#define IS_ADD_OR_SUBST(op) (op == E_OP_ADD || op == E_OP_R_SUBST)

inline const char* getOpString(EnumOp eOp)
{
    switch (eOp)
    {
    case E_OP_ADD: return "+";
    case E_OP_R_SUBST: return "-";
    case E_OP_MULTIPLE: return "*";
    default: return "/";
    }
}

struct SExpress
{
    SFraction oVal;
    std::string strExpress;
    EnumOp eOp;
    SFraction oL, oR;

    SExpress()
    {
        eOp = E_OP_NONE;
    }
};

class CBackwardCalcor
{
public:
    CBackwardCalcor(const std::vector<std::string>& vecStr);
    CBackwardCalcor(const std::vector<SExpress>& vecExp);
    void exec();

private:
    void workOnLevel(size_t seq1, size_t seq2);
    void stepOver(size_t& seq1, size_t& seq2);
    void reduct(EnumOp eOp, size_t seq1, size_t seq2, const std::vector<SExpress>& vecLeft);
    void doBiOp(EnumOp eOp, const SExpress& oExp1, const SExpress& oExp2, SExpress& oReduct, bool bNoLeft);
    bool isRedundant(EnumOp eOpL, EnumOp eOpR, EnumOp eOp);
    bool isRedundantEx(const SExpress& oExp1, const SExpress& oExp2, EnumOp eOp);

    void setAscendingOrder();
    bool higherLevel(size_t seq1, size_t seq2);
    bool topLevel(size_t seq1, size_t seq2);

    std::vector<SExpress> m_vecExpress;

    /// 头两个参与运算的数作为当前水位
    SFraction m_oCurValL; // 左数
    SFraction m_oCurValR; // 右数

public:
    static SFraction sm_oAnswer;
    static std::set<std::string> sm_setResult;

private:
    CBackwardCalcor(){};
};

