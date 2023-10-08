//
// Created by Yujie Zhao on 2023/10/7.
//

#ifndef QREGULAREXPRESSION_H
#define QREGULAREXPRESSION_H

#include <QtCore/qglobalstatic.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QRegularExpression
{
public:
    enum PatternOption {
        NoPatternOption                = 0x0000,
        CaseInsensitiveOption          = 0x0001,
        DotMatchesEverythingOption     = 0x0002,
        MultilineOption                = 0x0004,
        ExtendedPatternSyntaxOption    = 0x0008,
        InvertedGreedinessOption       = 0x0010,
        DontCaptureOption              = 0x0020,
        UseUnicodePropertiesOption     = 0x0040,
        // Formerly (no-ops deprecated in 5.12, removed 6.0):
        // OptimizeOnFirstUsageOption = 0x0080,
        // DontAutomaticallyOptimizeOption = 0x0100,
    };
    Q_DECLARE_FLAGS(PatternOptions, PatternOption)

    QRegularExpression() {}
    explicit QRegularExpression(const QString &pattern, PatternOptions options = NoPatternOption) {  }
};

QT_END_NAMESPACE

#endif //QREGULAREXPRESSION_H
