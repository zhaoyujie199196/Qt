#ifndef QUNICODETABLES_P_H
#define QUNICODETABLES_P_H

#include <QtCore/qchar.h>

QT_BEGIN_NAMESPACE

namespace QUnicodeTables
{
    enum Case {
        LowerCase,
        UpperCase,
        TitleCase,
        CaseFold,

        NumCases
    };

    // see http://www.unicode.org/reports/tr14/tr14-30.html
// we don't use the XX and AI classes and map them to AL instead.
    enum LineBreakClass {
        LineBreak_OP, LineBreak_CL, LineBreak_CP, LineBreak_QU, LineBreak_GL,
        LineBreak_NS, LineBreak_EX, LineBreak_SY, LineBreak_IS, LineBreak_PR,
        LineBreak_PO, LineBreak_NU, LineBreak_AL, LineBreak_HL, LineBreak_ID,
        LineBreak_IN, LineBreak_HY, LineBreak_BA, LineBreak_BB, LineBreak_B2,
        LineBreak_ZW, LineBreak_CM, LineBreak_WJ, LineBreak_H2, LineBreak_H3,
        LineBreak_JL, LineBreak_JV, LineBreak_JT, LineBreak_RI, LineBreak_CB,
        LineBreak_EB, LineBreak_EM, LineBreak_ZWJ,
        LineBreak_SA, LineBreak_SG, LineBreak_SP,
        LineBreak_CR, LineBreak_LF, LineBreak_BK,

        NumLineBreakClasses
    };

    struct Properties {
        ushort category  : 8;
        ushort direction : 8;
        ushort combiningClass: 8;
        ushort joining: 3;
        signed short digitValue: 5;
        signed short mirrorDiff: 16;
        ushort unicodeVersion: 8;
        ushort nfQuickCheck: 8;
#ifdef Q_OS_WASM
        unsigned char : 0;  //sasm 64 packing track
#endif
        struct {
            ushort special : 1;
            signed short diff : 15;
        } cases[NumCases];
#ifdef Q_OS_WASM
        unsigned char : 0;
#endif
        ushort graphemeBreakClass: 5;
        ushort wordBreakClass : 5;
        ushort lineBreakClass : 6;
        ushort sentenceBreakClass : 8;
        ushort script : 8;
    };

    LineBreakClass lineBreakClass(char32_t ucs4) noexcept;
    inline LineBreakClass lineBreakClass(QChar ch) noexcept {
        return lineBreakClass(ch.unicode());
    }
}

QT_END_NAMESPACE

#endif //QUNICODETABLES_P_H