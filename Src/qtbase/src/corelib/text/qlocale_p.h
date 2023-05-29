//
// Created by Yujie Zhao on 2023/5/16.
//

#ifndef QLOCALE_P_H
#define QLOCALE_P_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

struct QLocaleData
{
public:
    static const QLocaleData *c();

    enum DoubleForm {  //double数据的格式
        DFExponent = 0,  //科学计数法
        DFDecimal,  //十进制小数
        DFSignificantDigits,  //有效数字 DFExponent与DFDecimal中选择更加简明的那个
        _DFMax = DFSignificantDigits
    };

    enum Flags {  //zhaoyujie TODO
        NoFlags = 0,
        AddTrailingZeroes = 0x01,
        ZeroPadded = 0x02,
        LeftAdjusted = 0x04,
        BlankBeforePositive = 0x08,
        AlwaysShowSign = 0x10,
        GroupDigits = 0x20,
        CapitalEorX = 0x40,

        ShowBase = 0x80,
        UppercaseBase = 0x100,
        ZeroPadExponent = 0x200,
        ForcePoint = 0x400
    };

    enum NumberMode {
        IntegerMode,
        DoubleStandardMode,
        DoubleScientificMode
    };

public:
#define ForEachQLocaleRange(X) \
    X(startListPattern) X(midListPattern) X(endListPattern) X(pairListPattern) X(listDelimit) \
    X(decimalSeparator) X(groupDelim) X(percent) X(zero) X(minus) X(plus) X(exponential) \
    X(quoteStart) X(quoteEnd) X(quoteStartAlternate) X(quoteEndAlternate) \
    X(longDateFormat) X(shortDateFormat) X(longTimeFormat) X(shortTimeFormat) \
    X(longDayNamesStandalone) X(longDayNames) \
    X(shortDayNamesStandalone) X(shortDayNames) \
    X(narrowDayNamesStandalone) X(narrowDayNames) \
    X(anteMeridiem) X(postMeridiem) \
    X(byteCount) X(byteAmountSI) X(byteAmountIEC) \
    X(currencySymbol) X(currencyDisplayName) \
    X(currencyFormat) X(currencyFormatNegative) \
    X(endonymLanguage) X(endonymTerritory)

    //zhaoyujie TODO
    quint16 m_language_id;
    quint16  m_script_id;
    quint16 m_territory_id;
    char m_currency_iso_code[3];
    quint8 m_currency_digits : 2;
    quint8 m_currency_rounding : 3;  //未被使用过
    quint8 m_first_day_of_week : 3;
    quint8 m_weekend_start : 3;
    quint8 m_weekend_end : 3;
    quint8 m_grouping_top : 2;
    quint8 m_grouping_higher : 3;
    quint8 m_grouping_least : 3;
};

QT_END_NAMESPACE

#endif //QLOCALE_P_H
